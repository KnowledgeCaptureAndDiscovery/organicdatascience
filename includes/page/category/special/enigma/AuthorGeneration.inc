<?php

class AuthorGeneration extends SpecialPage {

  var $blocks;
  var $cache;

  public function __construct() {
    parent::__construct( 'AuthorGeneration' );
    $this->cache = array();
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
              "sort": false
            },
            {
              "id": "HasPSpecialContributor (E)",
              "name": "Special Contributor",
              "sort": true
            },
            {
              "id": "HasCohort (E).HasInvestigator (E)",
              "name": "Investigator",
              "sort": true
            },
            {
              "id": "HasCohort (E).HasPI (E)",
              "name": "PI",
              "sort": true
            },
            {
              "id": "HasPSeniorLead (E)",
              "name": "Senior Lead",
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
    $facts = array_key_exists($item, $this->cache) ?
      $facts = $this->cache[$item]:
      $facts = WTFactsAPI::getFacts($item)[1];
    $this->cache[$item] = $facts;

    # Pop the first element in the Property chain
    $prop = array_shift($prop_chain);
    if($prop) {
      if(array_key_exists($prop, $facts)) {
        $subfacts = array();
        foreach($facts[$prop]["values"] as $val) {
          $subitem = $val["text"];
          $subfacts = array_merge(
            $subfacts,
            $this->getItemFacts($subitem, $prop_chain, $filter_props)
          );
        }
        return $subfacts;
      }
    }
    else {
      # No further property. This should be the author, return details
      $details = array();
      foreach($filter_props as $key=>$filter_prop) {
        if(array_key_exists($filter_prop, $facts)) {
          $values = array();
          foreach($facts[$filter_prop]["values"] as $val) {
            $values[] = $val["text"];
          }
          $details[$key] = $values;
        }
      }
      return array($item=>$details);
    }
  }

  function getProjectAuthors($project) {
    global $wgCore;
    $blocks = $this->getAuthorBlocks($project);
    $props = $blocks["properties"];

    $authors = array();
    $filter_props = array(
      "title"=>"HasTitle $wgCore",
      "affiliations"=>"IsAffiliatedWith $wgCore"
    );
    foreach($props as $prop) {
      $prop_chain = explode(".", $prop["id"]);
      $group = $this->getItemFacts($project, $prop_chain, $filter_props);
      if($group) {
        if($prop["sort"])
          ksort($group);
        $group_name = $prop["name"];
        $authors[$group_name] = $group;
      }
    }
    return $authors;
  }

  function getAuthorsHTML($proj_name) {
    $unique_authors = array();
    $author_hash = array();
    $affiliations = array();

    // Get all project authors
    $authors = $this->getProjectAuthors($proj_name);

    // Extract unique affiliations and authors
    foreach($authors as $group_name=>$group_authors) {
      $unique_authors[$group_name] = array();
      foreach($group_authors as $author_name=>$author) {
        // Gather all affiliations
        if(array_key_exists('affiliations', $author)) {
          foreach($author['affiliations'] as $aff) {
            if(!in_array($aff, $affiliations)) {
              $affiliations[] = $aff;
            }
          }
        }
        if(!array_key_exists($author_name, $author_hash)) {
          $unique_authors[$group_name][$author_name] = $author;
          $author_hash[$author_name] = $author;
        }
      }
    }
    // sort($affiliations);

    // Create Authors Text
    $authors_text = "";
    foreach(array_keys($authors) as $group_name) {
      // Format the authors in display format
      foreach($unique_authors[$group_name] as $author_name=>$author) {
        if($authors_text) {
          $authors_text .= ", ";
        }
        $authors_text .= $author_name;
        if(array_key_exists("affiliations", $author)) {
          $affnums = implode(",",
            array_map(
              function($a) use ($affiliations){
                return array_search($a, $affiliations) + 1;
              },
              $author['affiliations']
            ));
          $authors_text .= "<sup>$affnums</sup>";
        }
      }
    }

    // Create Affiliations Text
    $affiliations_text = "";
    foreach($affiliations as $aff) {
      $affiliations_text .= "<li>$aff</li>";
    }

    // Create Contributions Text
    $contribution_text = "";

    $text1 = " are the ";
    $text3 = " is the ";
    $text2 = " of the project";
    foreach($authors as $group_name=>$group_authors) {
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
        <div class='panel-heading'><span style='font-weight: bold;'>Contribution</span></div>
        <div class='panel-body'>$contribution_text</div>
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
