<?php

class AuthorGeneration extends SpecialPage {

  var $blocks;
  var $cache;
  var $authors;
  var $groups;

  public function __construct() {
    parent::__construct( 'AuthorGeneration' );
    $this->cache = array();
    $this->authors = array();
    $this->groups = array();
  }

  function execute( $par ) {
    global $wgDir, $wgUser;
    $request = $this->getRequest();
    $out = $this->getOutput();
    $out->addExtensionStyle( $wgDir . '/css/wtree.css' );
    $this->setHeaders();
    $this->handleOperation($request, $out);
  }

  function handleOperation($request, $out) {
    global $wgUser;
    $project = $request->getText("project");
    if($project) {
      $html = $this->getAuthorsHTML($project);
      $out->addHTML($html);
    } else {
      $html = '
      <form>
      Project Name: <input name="project" />
      <input type="submit" />
      </form>
      ';
      $out->addHTML($html);
    }
  }

  function getAuthorBlocks($project) {
    $rules_page = "AuthorRules:$project";
    $titobj = Title::newFromText($rules_page);
    $page = new WikiPage($titobj);
    $json = null;
    if(!$titobj->exists()) {
      $json = '
      {
         "properties": [
            {
              "id": "HasPJuniorLead (E)",
              "name": "Junior Lead",
              "priority": 1,
              "sort": false
            },
            {
              "id": "HasPSpecialContributor (E)",
              "name": "Special Contributor",
              "priority": 2,
              "sort": true
            },
            {
              "id": "HasCohort (E).HasInvestigator (E)",
              "name": "Investigator",
              "priority": 3,
              "sort": true
            },
            {
              "id": "HasCohort (E).HasPI (E)",
              "name": "PI",
              "priority": 4,
              "sort": true
            },
            {
              "id": "HasPSeniorLead (E)",
              "name": "Senior Lead",
              "priority": 5,
              "sort": false
            }
         ]
      }';
      $content = ContentHandler::makeContent($json, $titobj);
      $status = $page->doEditContent($content, "Initialized default author generation rules");
      assert($status->isOK());
    }
    else {
      $json = $page->getContent()->serialize();
    }
    return json_decode($json, true);
  }

  function getItemFacts($item, $prop_chain=array(), $filter_props=array()) {
    # Get facts about the item
    $data = array_key_exists($item, $this->cache) ?
      $data = $this->cache[$item]:
      $data = array(
			"facts"=>WTFactsAPI::getFacts($item)[1],
			"inprops"=>WTFactsAPI::getInSubjectProperties($item)
		);
    $this->cache[$item] = $data;

    # Pop the first element in the Property chain
    $prop = array_shift($prop_chain);
    if($prop) {
      $values = $this->getPropertyValues($data, $prop);
      $subfacts = array();
      foreach($values as $val) {
        $subfacts = array_merge(
          $subfacts,
          $this->getItemFacts($val, $prop_chain, $filter_props)
        );
      }
      return $subfacts;
    }
    else {
      # No further property. This should be the author, return details
      $details = array();
      foreach($filter_props as $key=>$filter_prop) {
        $details[$key] = $this->getPropertyValues($data, $filter_prop);
      }
      return array($item=>$details);
    }
  }

  function getPropertyValues($data, $prop) {
    $facts = $data["facts"];
    $inprops = $data["inprops"];

	 list($prop, $extra_info) = explode("|", $prop);
    $values = [];
    if($prop[0]=="-") {
      $iprop = substr($prop, 1);
      foreach($inprops as $inprop) {
        if($inprop["property"] == $iprop) {
          $values[] = str_replace("_", " ", $inprop["subject"]);
        }
      }      
    }
    else {
      if(array_key_exists($prop, $facts)) {
        foreach($facts[$prop]["values"] as $val) {
          $values[] = str_replace("_", " ", $val["text"]);
        }
      }
    }
	 if($extra_info) {
      $nvalues = [];
      foreach($values as $val) {
			$facts = WTFactsAPI::getFacts($val)[1];
         if(array_key_exists($extra_info, $facts)) {
           foreach($facts[$extra_info]["values"] as $evalue) 
             $val .= "|".$evalue["text"];
         }
			$nvalues[] = $val;
      }
      return $nvalues;
    }
    return $values;
  }

  function getProjectAuthors($project) {
    global $wgCore;
    $blocks = $this->getAuthorBlocks($project);
    $props = $blocks["properties"];
    $filter_props = array(
      "title"=>"HasTitle $wgCore",
      "first_name"=>"GivenName $wgCore",
      "middle_initial"=>"HasMiddleInitial $wgCore",
      "last_name"=>"FamilyName $wgCore",
      "coi"=>"HasConflictOfInterest $wgCore",
      "grant"=>"HasGrant $wgCore",
      "affiliations"=>"-HasAffiliate $wgCore|IsPartOfOrganization $wgCore"
    );

    # Sort groups/roles by group priority
    $priority_props = $props;
    usort($priority_props, function($a, $b) { return $a["priority"] - $b["priority"]; });

    # Get "merged" groups and sorted authors
    $merged_groups = array();
    $authors = array();
    $author_hash = array();
    foreach($props as $prop) {
      $this->groups[$prop["id"]] = $prop["name"];
      $prop_chain = explode(".", $prop["id"]);
      $group_authors = $this->getGroupAuthors($prop["id"], $project, $prop_chain, $filter_props);
      $sort_group = $prop["sort_as"] ? $prop["sort_as"] : $prop["id"];
      if(!$authors[$sort_group]) {
        $authors[$sort_group] = array();
        $merged_groups[] = $sort_group;
      }
      $authors[$sort_group] = array_unique(array_merge($authors[$sort_group], $group_authors));
      if($prop["sort"])
        sort($authors[$sort_group]);
    }

    # Get author main group/role
    foreach($this->authors as $author_name=>$author) {
      foreach($priority_props as $prop) {
        if(in_array($prop["id"], $author["groups"])) {
          $sort_group = $prop["sort_as"] ? $prop["sort_as"] : $prop["id"];
          $this->authors[$author_name]["main_group"] = $sort_group;
          break;
        }
      }
    }
    return $authors;
  }

  function getGroupAuthors($group_name, $project, $prop_chain, $filter_props) {
    $group_authors = array();
    $authors = $this->getItemFacts($project, $prop_chain, $filter_props);
    # TODO: set author_name according to AuthorRules for naming
    foreach($authors as $page_name=>$author) {
      $author_name = $this->getAuthorName($page_name, $author);
      $author["page_name"] = $page_name;
      $eauthor = $this->authors[$author_name];
      if(!$eauthor) {
        $eauthor = $author;
        $eauthor["groups"] = array();
      }
      if(!in_array($group_name, $eauthor["groups"]))
      	$eauthor["groups"][] = $group_name;
      $this->authors[$author_name] = $eauthor;
      $group_authors[] = $author_name;
    }
    return $group_authors;
  }
  
  function getAuthorName($page_name, $author) {
    $author_name = $page_name;
    if($author["last_name"] && $author["first_name"]) {
      $author_name = $author["last_name"][0] . " " . $author["first_name"][0];
      if($author["middle_initial"])
        $author_name .= " " . $author["middle_initial"][0];
    }
    return $author_name;
  }

  function getAuthorsHTML($proj_name) {
    global $wgScriptPath;
    $unique_authors = array();
    $author_hash = array();
    $affiliations = array();
    $fullaffiliations = array();

    // Get all project author names
    $author_names = $this->getProjectAuthors($proj_name);

    // For each group, get authors that have that as the main group
    foreach($author_names as $group_name=>$group_author_names) {
      $unique_authors[$group_name] = array();
      foreach($group_author_names as $author_name) {
        $author = $this->authors[$author_name];
        if($author["main_group"] == $group_name)
          $unique_authors[$group_name][$author_name] = $author;
      }
    }

    // Extract unique affiliations
    foreach($author_names as $group_name=>$group_author_names) {
      foreach($group_author_names as $author_name) {
        $author = $this->authors[$author_name];
        // Gather all affiliations
        if(array_key_exists('affiliations', $author)) {
          foreach($author['affiliations'] as $aff) {
				$affarr = explode("|", $aff);
				$afftxt = implode(", ", $affarr);
            if(!in_array($afftxt, $fullaffiliations)) {
              $affiliations[] = $affarr[0];
              $fullaffiliations[] = $afftxt;
            }
          }
        }
      }
    }
    // sort($affiliations);

    // Create Authors Text
    $authors_text = "";
    foreach($author_names as $group_name=>$group_author_names) {
      // Format the authors in display format
      foreach($unique_authors[$group_name] as $author_name=>$author) {
        $page_name = $author["page_name"];
        if($authors_text) {
          $authors_text .= ", ";
        }
        $authors_text .= "<a href='$wgScriptPath/index.php/$page_name'>$author_name</a>";
        if(array_key_exists("affiliations", $author)) {
          $affnums = implode(",",
            array_map(
              function($a) use ($affiliations){
                $arr = explode("|", $a);
                return array_search($arr[0], $affiliations) + 1;
              },
              $author['affiliations']
            ));
          $authors_text .= "<sup>$affnums</sup>";
        }
      }
    }

    // Create Affiliations Text
    $affiliations_text = "";
    for($i=0; $i<sizeof($affiliations); $i++) {
      $fullaff = $fullaffiliations[$i];
      $aff = $affiliations[$i];
      $affiliations_text .= "<li><a href='$wgScriptPath/index.php/$aff'>$fullaff</a></li>";
    }

    // Create Contributions Groups
    $contrib_group_authors = array();
    foreach($this->groups as $group_id=>$group_name) {
      $contrib_group_authors[$group_name] = array();
    }
    foreach($author_names as $group_name=>$group_author_names) {
      foreach($group_author_names as $author_name) {
        $contributor = $this->authors[$author_name];
        foreach($contributor["groups"] as $cgroup) {
          $cgroup_name = $this->groups[$cgroup];
          $contrib_group_authors[$cgroup_name][$author_name] = $contributor;
        }
      }
    }

    // Create Contributions Text
    $contribution_text = "";
    foreach($contrib_group_authors as $group_name=>$group_authors) {
      $temp = "";
      foreach($group_authors as $author_name=>$contributor) {
        if($temp) {
          $temp .= ", ";
        }
        if(preg_match("/^[A-Z0-9]+$/", $author_name))// An acronym
          $temp .= $author_name;
        else
          $temp .= $this->getInitials($author_name);
      }
      if($temp) {
        if($contribution_text) {
          $contribution_text .= ". ";
        }
        $contribution_text = $contribution_text . $temp;
        if(sizeof($authors[$group_name]) == 1) {
          $contribution_text .= " is the ". $group_name;
        }
        else {
          $gtext = $group_name . 's';
          if(preg_match("/(.+)ium$/i", $group_name, $m)) {
            $gtext = $m[1] . "ia";
          }
          $contribution_text .= " are the " . $gtext;
        }
        $contribution_text .= " of the project";
      }
    }

    // Create Conflict Of Interests Text
    $coi_text = "";
    foreach($unique_authors as $group_name=>$group_authors) {
      foreach($group_authors as $author_name=>$contributor) {
        if($contributor["coi"]) {
          $coi_text .= $author_name . " declares a conflict of interest: \"".$contributor["coi"][0]."\". ";
        }
      }
    }
    $coi_text .= "All " . ($coi_text ? "other " : "") . "authors have declared no financial or ";
    $coi_text .= "commercial conflicts of interest with this work";


    // Create Acknowledgements Text
    $ack_text = "";
    foreach($unique_authors as $group_name=>$group_authors) {
      foreach($group_authors as $author_name=>$contributor) {
        if($contributor["grant"]) {
          if($ack_text) 
            $ack_text .= ", ";
          $ack_text .= $contributor["grant"][0]. " (". $this->getInitials($author_name). ")";
        }
      }
    }

    return "
    <h2>Project: $proj_name</h2>
    <div class='panel panel-default'>
        <div class='panel-heading'><span style='font-weight: bold;'>Author list</span></div>
        <div class='panel-body'>$authors_text</div>
    </div>
    <div class='panel panel-default'>
        <div class='panel-heading'><span style='font-weight: bold;'>Affiliation List</span></div>
        <div class='panel-body'>
          <ol id='Affiliations'>$affiliations_text</ol>
        </div>
    </div>
    <div class='panel panel-default'>
        <div class='panel-heading'><span style='font-weight: bold;'>Contributions</span></div>
        <div class='panel-body'>$contribution_text</div>
    </div>
    <div class='panel panel-default'>
        <div class='panel-heading'><span style='font-weight: bold;'>Conflicts of Interest</span></div>
        <div class='panel-body'>$coi_text</div>
    </div>
    <div class='panel panel-default'>
        <div class='panel-heading'><span style='font-weight: bold;'>Acknowledgements</span></div>
        <div class='panel-body'>$ack_text</div>
    </div>
    ";
  }

  function getInitials($name) {
    $initials = explode(' ', $name);
    foreach ($initials as $key => $value) {
        $initials[$key] = $value[0] . '.';
    }
    return implode(" ", $initials);
  }

  function getGroupName() {
    return "ODSGroup";
  }

}
