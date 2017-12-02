<?php

class AuthorGeneration extends SpecialPage {

	public function __construct() {
		parent::__construct( 'AuthorGeneration' );
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

  function getProjectAuthors($project) {
    global $wgCore;
    $JuniorLead = "HasPJuniorLead " . $wgCore;
    $SeniorLead = "HasPSeniorLead " . $wgCore;
    $SpecialContributor = "HasPSpecialContributor " . $wgCore;

    $authors = array();
    $proj_facts = WTFactsAPI::getFacts($project)[1];
    $authors = $this->extractAuthors($proj_facts, $authors, $JuniorLead, "Junior Lead");
    $authors = $this->extractAuthors($proj_facts, $authors, $SeniorLead, "Senior Lead");
    $authors = $this->extractAuthors($proj_facts, $authors, $SpecialContributor, "Special Contributor");
    $authors = $this->extractCohortAuthors($proj_facts, $authors);
    $authors = $this->getAuthorDetails($authors);

    return $authors;
  }

  function extractAuthors($facts, $authors, $roleprop, $role) {
    global $wgCore;
    if(array_key_exists($roleprop, $facts)) {
      foreach($facts[$roleprop]['values'] as $val) {
        $author_name = $val["text"];
        $authors = $this->appendAuthor($authors, $author_name, $role);
      }
    }
    return $authors;
  }

  function extractCohortAuthors($proj_facts, $authors) {
    global $wgCore;
    $PCohort = "HasCohort " . $wgCore;
    $PI = "HasPI " . $wgCore;
    $Investigator = "HasInvestigator " . $wgCore;

    if(array_key_exists($PCohort, $proj_facts)) {
      $cohort_names = array();
      foreach($proj_facts[$PCohort]['values'] as $citem) {
        $cohort_names[] = $citem['text'];
      }
      foreach($cohort_names as $cohort_name) {
        $cohort_facts = WTFactsAPI::getFacts($cohort_name)[1];
        $authors = $this->extractAuthors($cohort_facts, $authors, $PI, "PI");
        $authors = $this->extractAuthors($cohort_facts, $authors, $Investigator, "Investigator");
      }
    }
    return $authors;
  }

  function appendAuthor($authors, $author_name, $role) {
    $author = array_key_exists($author_name, $authors) ?
        $authors[$author_name] :
        array("Name"=>$author_name, "Roles"=>array());
    $author["Roles"][] = $role;
    $authors[$author_name] = $author;
    return $authors;
  }

  function getAuthorDetails($authors) {
    global $wgCore;
    $Title = "HasTitle " . $wgCore;
    $Affiliation = "IsAffiliatedWith " . $wgCore;

    foreach($authors as $author_name=>$author) {
      $author_facts = WTFactsAPI::getFacts($author_name)[1];
      $author["Qual"] = "";
      if(array_key_exists($Title, $author_facts)) {
        $author["Qual"] = $author_facts[$Title]['values'][0]['text'];
      }
      $author["Affiliations"] = [];
      if(array_key_exists($Affiliation, $author_facts)) {
        foreach($author_facts[$Affiliation]['values'] as $aff) {
          $author["Affiliations"][] = $aff['text'];
        }
      }
      $authors[$author_name] = $author;
    }
    return $authors;
  }

  function getAuthorsHTML($proj_name) {
    $author_list = array();
    $affiliation_list = array();
    $contributors_list = array();

    // Author list priority/hierarchy
    $hierarchy = array("Junior Lead", "Special Contributor", "Investigator", "PI", "Senior Lead");

    // Get all project authors
    $authors = $this->getProjectAuthors($proj_name);

    // Create object of empty list for each role type
    foreach($hierarchy as $hieritem) {
        $author_list[$hieritem] = array();
        $contributors_list[$hieritem] = array();
    }

    // Extract information from authors
    foreach($authors as $author_name=>$author) {
      // Gather all affiliations
      if(array_key_exists('Affiliations', $author)) {
        foreach($author['Affiliations'] as $aff) {
          if(!in_array($aff, $affiliation_list)) {
            $affiliation_list[] = $aff;
          }
        }
      }
      //  Get the most important role of the author based on hierarchy in hierarchy list
      $flag = false;
      foreach($hierarchy as $hieritem) {
        if(in_array($hieritem, $author['Roles'])) {
          $contributors_list[$hieritem][] = $author_name;
          if (!$flag) {
            $author['Role'] = $hieritem;
            $author_list[$hieritem][] = $author;
            $flag = true;
          }
        }
      }
    }

    // Sort affiliation list and display as list
    sort($affiliation_list);
    $affiliations_text = "";
    foreach($affiliation_list as $aff) {
      $affiliations_text .= "<li>$aff</li>";
    }

    // Adding contribution section based on the roles as no data is available regarding the exact workof the author
    $roles_text = "";

    $text1 = " are the ";
    $text3 = " is the ";
    $text2 = " of the project";
    foreach($hierarchy as $hieritem) {
      $temp = "";
      foreach($contributors_list[$hieritem] as $contributor) {
        if($temp) {
          $temp .= ", ";
        }
        $initials = explode(' ', $contributor);
        foreach ($initials as $key => $value) {
            $initials[$key] = $value[0] . '.';
        }
        $temp .= implode(" ", $initials);
      }
      if($temp) {
        if($roles_text) {
          $roles_text .= ". ";
        }
        $roles_text = $roles_text . $temp;
        if(sizeof($contributors_list[$hieritem]) == 1) {
          $roles_text = $roles_text . $text3 . $hieritem . $text2;
        }
        else {
          $roles_text = $roles_text . $text1 . $hieritem . 's' . $text2;
        }
      }
    }

    $authors_text = "";
    foreach($hierarchy as $hieritem) {
      // The Junior and Senior leads should not be alphabetically sorted
      if(!preg_match("/(Junior|Senior) Lead/", $hieritem)) {
        $names = array();
        foreach ($author_list[$hieritem] as $author) {
          $names[] = $author['Name'];
        }
        array_multisort($names, SORT_ASC, $author_list[$hieritem]);
      }
      // Format the authors in display format
      foreach($author_list[$hieritem] as $author) {
        $temp = "";
        foreach($author['Affiliations'] as $aff) {
          if($temp) {
            $temp .= ", ";
          }
          $temp .= array_search($aff, $affiliation_list) + 1;
        }
        if($authors_text) {
          $authors_text .= ", ";
        }
        $qual = $author['Qual'];
        $qual = $qual ? " $qual" : "";
        $authors_text .= $author['Name'] . $qual . "<sup>" . $temp . "</sup>";
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
        <div class='panel-body'>$roles_text</div>
    </div>
    ";
  }

	function getGroupName() {
		return "ODSGroup";
	}

}
