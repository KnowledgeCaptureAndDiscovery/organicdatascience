<?php

class WTBase {
	var $title;

	function __construct($title) {
		$this->title = $title;
	}

	function getItemLink($item) {
		$title = $item->getTitle();
		$url = $title->getInternalURL();
		$text = $title->getText();
		$cls = $title->exists() ? '' : 'new';
		return "<a class='$cls' href='$url'>$text</a>";
	}


	function getContributors($title, $allow_anon=false) {
		$conds['rev_page'] = $title->getArticleId();
		$conds[] = 'rev_deleted & ' . Revision::DELETED_USER . ' = 0';
		if(!$allow_anon) $conds[] = 'rev_user != 0'; 
	
		$contributors = array();
		$dbr = wfGetDB( DB_SLAVE );
		$res = $dbr->select(
			'revision',
			array(
				'COUNT(*) AS count',
				'rev_user',
				'rev_user_text',
			),
			$conds,
			__METHOD__,
			array(
				'GROUP BY' => 'rev_user_text',
				'ORDER BY' => 'count DESC'
			)
		);
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while( $row = $dbr->fetchObject( $res ) )
				$contributors[ $row->rev_user_text ] = array( $row->rev_user, $row->count );
		}
		return $contributors;
	}

	function mergeContributors($c1s, $c2s) {
		$newcs = array_merge($c1s, array_diff_key($c2s, $c1s));
		$intersection = array_intersect_key($c2s, $c1s);
		foreach($intersection as $key=>$val) {
			$newcs[$key][1] += $c2s[$key][1];
		}
		return $newcs;
	}

	public static function compareContributors($a,$b) {
		if($a[1]==$b[1]) return 0; 
		return ($a[1]<$b[1]) ? 1 : -1;
	}

	function getPageDetails($title=null) {
		global $wgAnonCredit;
		if(!$title) $title = $this->title;
		$details = array();
		$details["Contributors"] = $this->getContributors($title, $wgAnonCredit);
		uasort($details["Contributors"], "WTBase::compareContributors");
		return $details;
	}


	
	function includeJSHeaders($out, $wgDir) {
		$out->addModules('WorkflowTasks');
	}

	function includeCSSHeaders($out, $wgDir) {
		$out->addExtensionStyle( $wgDir . '/css/wtree.css' );
	}

	function addCategoryHeaders($out, $cats) {
		$old = $out->getHTML();
		$out->clearHTML();
		if($cats and sizeof($cats) > 0)
			$out->addHTML("<div class='subheader'>( ". implode(", ", $cats)." )</div>");
		else
			$out->addHTML("<style>h1.firstHeading { border-bottom: 2px solid #C9302C; }</style>");
		# -- Add Existing Markup Here
		$out->addHTML( $old );
	}

	function setJavascriptVariables($out, $nofacts=false) {
		global $wgCore;
		global $wgUseSimpleTasks, $wgDisableTracking;
		$out->addHTML("<script>\n"); 
		if(!$nofacts) {
			list ($error, $facts, $text) = WTFactsAPI::getFacts($this->title->getFullText());
			$inprops = WTFactsAPI::getInSubjectProperties($this->title->getFullText());
			$out->addHTML("var allwtfacts = ".json_encode($facts).";\n");
			$out->addHTML("var allinprops = ".json_encode($inprops).";\n");
		}
		else {
			$out->addHTML("var allwtfacts = [];\n");
			$out->addHTML("var allinprops = [];\n");
		}
		$details = $this->getPageDetails();
		$out->addHTML("var allwtdetails = ".json_encode($details).";\n");
		$out->addHTML("var wtpagenotfound = ".($this->title->exists() ? 'false' : 'true').";\n");
		$out->addHTML("var wtallcategories = [];\n");
		$out->addHTML("var wgCore = '$wgCore';\n");
		//$out->addHTML("var wtallcategories = ['SampleCollection','Procedure','Workflow', 'ExecutedWorkflow', 'Task', 'Answer', 'AutomaticallyProvidedData', 'UserProvidedData', 'UserDescribedData', 'Component', 'Model', 'Person', 'Object', 'DataType', 'Thing'];\n");
		//$out->addHTML("var wtallcategories = ['UserProvidedData', 'UserDescribedData', 'Person', 'DataType', 'Thing'];\n");
		if(!$wgUseSimpleTasks) {
			$explorer = new WTTaskExplorer($this->title);
			$out->addHTML("var allwtexplorer = ".json_encode($explorer->getExplorer()).";\n");
		}
		$out->addHTML("var wtcategories = [];\n");
		$out->addHTML("var stdwtprops = [];\n"); 
		$cats = $out->getCategories();
		$props = [];
		foreach($cats as $cat) {
			$out->addHTML("wtcategories['$cat'] = true;\n"); 
			if(!$nofacts)
 				$props = array_merge($props, WTFactsAPI::getCategoryProperties($cat)); 
		}
		if(!$nofacts)
			$out->addHTML("stdwtprops = ".json_encode($props).";\n"); 

		global $wgUser;	 
 		$out->addHTML("var wtuid = null;\n"); 
 		if($wgUser->isLoggedIn()){ 
 			$uid = $wgUser->getName(); 
 			$out->addHTML("wtuid = '".$uid."';\n"); 
 		} 

		$rightsmap = array();
		foreach($wgUser->getRights() as $right)
			$rightsmap[$right] = 1;
		$out->addHTML("var wtrights = ". json_encode($rightsmap).";\n");

		$out->addHTML("var __use_simple_tasks = ".($wgUseSimpleTasks ? 'true' : 'false').";\n");
		$out->addHTML("var __disable_tracking = ".($wgDisableTracking ? 'true' : 'false').";\n");
		$out->addHTML("</script>\n"); 
	}

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();
		
		# -- Add the Standard Properties Table 
		$this->addStdPropsDiv( $out ); 

		# -- Add Existing Markup Here
		$out->addHTML( $old );
				
		# -- Add the Facts Table
		$this->addMainFactsDiv( $out ); 

		# -- Add the in-properties Table
		$this->addInPropsDiv( $out );

		# -- Credits Div
		$this->addCreditsDiv( $out );

		# -- Sidebar Div
		$this->addSidebarDiv( $out );
		
		# -- Add the Category Changer
		$this->addCategoryChooserDiv( $out );
	}

	function addCategoryChooserDiv($out) {
		$out->addHTML("<div id='category-chooser' class='props-table'></div>");
	}

	function addTreeDiv($out, $type) {
		global $wgUseSimpleTasks;
		if($wgUseSimpleTasks)
			$out->addHTML("<div id='main-simple-tree' class='props-table'></div>");
		else
			$out->addHTML("<div id='main-tree' class='customtree'></div>");
	}

	function addStdPropsDiv($out) { 
		$out->addHTML("<div id='main-std-props' class='props-table'></div>"); 
	} 

	function addInPropsDiv($out) { 
		$out->addHTML("<div id='main-in-props' class='props-table'></div>"); 
	} 	

	function addTimelineDiv($out, $type) {
		$out->addHTML("<div id='main-timeline' class='props-table'></div>");
	}

	function addTaskMetaDataDiv($out) {
		$out->addHTML("<div id='main-taskmetadata'></div>");
	}
	
	function addFactsDiv($out) {
		$this->addStdPropsDiv($out);
		$this->addMainFactsDiv($out);
		$this->addInPropsDiv($out);
	}

	function addMainFactsDiv($out) {
		$out->addHTML("<div id='main-facts' class='props-table'></div>");
	}

	function addSidebarDiv($out) {
		$out->addHTML("<div id='main-tree-sidebar'></div>");
		global $wgUseSimpleTasks, $wgDefaultSkin;
		if(!$wgUseSimpleTasks && strtolower($wgDefaultSkin)=="vector") {
			/** HACK! Overriding styles for sidebar adjustment*/
			$style = '<style id="main-tree-sidebar-style">';
			$style .= '#content-resize-btn{ margin-left: 230px !important;}';
			$style .= '#content, #footer, #left-navigation{ margin-left:270px !important; }';
			$style .= '#p-navigation{ width: 251px !important; }';
			$style .= '</style>';
			$out->addHTML($style);
		}
	}

	function addCreditsDiv($out) {
		$out->addHTML("<div id='main-credits' class='props-table'></div>");
	}

	function addAnswersDiv($out) {
		$out->addHTML("<div id='main-answers' class='props-table'></div>");
	}

	function addTasksDiv($out) {
		$out->addHTML("<div id='main-tasks' class='props-table'></div>");
	}

	function addWorkflowDiv($out) {
		$out->addHTML("<div id='main-workflow' class='props-table'></div>");
	}

	function addDataDiv($out) {
		$out->addHTML("<div id='main-data' class='props-table'></div>");
	}

	function addPersonDiv($out, $nestedprops=false) {
		$out->addHTML("<div id='main-person' class='props-table'>");
		if($nestedprops)
			$this->addStdPropsDiv($out);
		$out->addHTML("</div>");
	}

	function addPersonExpertiseDiv($out) {
		$out->addHTML("<div id='main-personexpertise'></div>");
	}
	
	function addPersonTasksDiv($out) {
		$out->addHTML("<div id='main-persontasks'></div>");
	}
	
	function addDocuDiv($out) {
		$out->addHTML("<div id='main-docu'></div>");
	}
	
	function addAdminDiv($out) {
		$out->addHTML("<div id='main-admin'></div>");
	}
	
	function addComponentDiv($out) {
		$out->addHTML("<div id='main-comp' class='props-table'></div>");
	}
	
	function addYandexMetrica($out) {
		global $wgUseSimpleTasks;
		global $wgDisableTracking;
		global $wgCanonicalServer;
		global $wgExtensionAssetsPath;		
		if(!$wgUseSimpleTasks && !$wgDisableTracking) {
			$url = $wgCanonicalServer.$wgExtensionAssetsPath.'/WorkflowTasks/includes/tracking/YandexMetrica.html';
			$out->addHTML(file_get_contents($url));
		}
	}
}

?>
