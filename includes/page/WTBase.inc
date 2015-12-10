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
		$out->includeJQuery();
		$out->addScriptFile( '//code.jquery.com/ui/1.11.2/jquery-ui.js' ); 
		$out->addScriptFile( '//cdnjs.cloudflare.com/ajax/libs/jquery-noty/2.3.7/jquery.noty.min.js' ); 
		$out->addScriptFile( '//cdnjs.cloudflare.com/ajax/libs/jquery-noty/2.3.7/layouts/top.min.js' ); 
		$out->addScriptFile( '//cdnjs.cloudflare.com/ajax/libs/jquery-noty/2.3.7/themes/relax.min.js' ); 
		//$out->addScriptFile( $wgDir . '/js/lib/jquery/jquery.jeditable.js' );
		$out->addScriptFile( $wgDir . '/js/lib/jquery/jquery.loadmask.min.js' );
		$out->addScriptFile( $wgDir . '/js/lib/jquery/jquery.autocomplete.js' );
		$out->addScriptFile( $wgDir . '/js/lib/jquery/jquery.autocomplete2.js' );
		$out->addScriptFile( $wgDir . '/js/lib/jquery/jquery.tooltipster.min.js' );
		$out->addScriptFile( $wgDir . '/js/lib/jquery/jquery.picker.js' );
		$out->addScriptFile( $wgDir . '/js/lib/jquery/jquery.picker.date.js' );
		$out->addScriptFile( $wgDir . '/js/lib/jquery/jquery.lightbox_me.js' );
		$out->addScriptFile( $wgDir . '/js/lib/moment/moment.min.js' );
		//$out->addScriptFile( $wgDir . '/js/lib/jquery/jquery.translate-NCT,core,NCT-adapter,ui,progress.min.js' );
		//$out->addScriptFile( $wgDir . '/js/lib/jquery/jqModal.js' );
		$out->addScriptFile( $wgDir . '/js/lib/jstree/jstree.js' );
		$out->addScriptFile( $wgDir . '/js/lib/raphael/raphael-min.js' );
		
		$out->addScriptFile( $wgDir . '/includes/tracking/WTTracker.js' );
		
		$out->addScriptFile( $wgDir . '/js/core/api/WTAPI.js' );						
		$out->addScriptFile( $wgDir . '/js/core/api/WTExplorerParser.js' );
		$out->addScriptFile( $wgDir . '/js/core/api/WTExplorerAPI.js' );		
		$out->addScriptFile( $wgDir . '/js/core/util/WTUtil.js' );
		$out->addScriptFile( $wgDir . '/js/core/ui/WTSidebar.js' );
		$out->addScriptFile( $wgDir . '/js/core/ui/WTExplorerMenu.js' );
		$out->addScriptFile( $wgDir . '/js/core/ui/WTExplorerEvents.js' );
		$out->addScriptFile( $wgDir . '/js/core/ui/WTSidebarResizer.js' );		
		$out->addScriptFile( $wgDir . '/js/core/ui/WTTimeline.js' );		
		$out->addScriptFile( $wgDir . '/js/core/ui/WTTaskAlert.js' );
		$out->addScriptFile( $wgDir . '/js/core/ui/WTDialog.js' );		
		
		$out->addScriptFile( $wgDir . '/js/page/component/WTFacts.js' );
		$out->addScriptFile( $wgDir . '/js/page/component/WTStdProperties.js' ); 
		$out->addScriptFile( $wgDir . '/js/page/component/WTCredits.js' );
		$out->addScriptFile( $wgDir . '/js/page/component/WTCategoryChooser.js' );		
		$out->addScriptFile( $wgDir . '/js/page/component/WTDataColumns.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/task/WTTasks.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/task/WTTaskContext.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/task/WTSubTasks.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/task/WTSimpleTasks.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/task/WTTaskMetaData.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/answer/WTAnswers.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/data/WTData.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/data/WTUserProvidedData.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/data/WTUserDescribedData.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/workflow/WTComponent.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/workflow/WTWorkflow.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/workflow/WTExecutedWorkflow.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/person/WTPerson.js' );		
		$out->addScriptFile( $wgDir . '/js/page/category/person/WTPersonExpertise.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/person/WTPersonContext.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/person/WTPersonTasks.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/special/WTDocu.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/special/WTAdmin.js' );
				
		$out->addScriptFile( $wgDir . '/js/main.js' );
		
	}

	function includeCSSHeaders($out, $wgDir) {
		$out->addExtensionStyle( '//maxcdn.bootstrapcdn.com/font-awesome/4.4.0/css/font-awesome.min.css' ); 
		$out->addExtensionStyle( $wgDir . '/css/jquery/jquery.loadmask.css' );			
		$out->addExtensionStyle( '//code.jquery.com/ui/1.11.2/themes/flick/jquery-ui.css'); 
		$out->addExtensionStyle( $wgDir . '/css/jquery/jquery.autocomplete.css' );
		$out->addExtensionStyle( $wgDir . '/css/jquery/jquery.loadmask.css' );		
		$out->addExtensionStyle( $wgDir . '/css/jquery/jquery.tooltipster.css');
		$out->addExtensionStyle( $wgDir . '/css/jquery/jquery.tooltipster.custom.css');
		$out->addExtensionStyle( $wgDir . '/css/jquery/jquery.picker.classic.css');
		$out->addExtensionStyle( $wgDir . '/css/jquery/jquery.picker.classic.date.css');
		$out->addExtensionStyle( $wgDir . '/css/jquery/jquery.picker.classic.date.custom.css');
		$out->addExtensionStyle( $wgDir . '/css/wtree.css' );
		$out->addExtensionStyle( $wgDir . '/css/jstree/style.css');
		$out->addExtensionStyle( $wgDir . '/css/jstree/style.custom.css');
	}

	function setJavascriptVariables($out) {
		$details = $this->getPageDetails();
		list ($error, $facts, $text) = WTFactsAPI::getFacts($this->title->getFullText());
		$out->addHTML("<script>\n"); 
		$out->addHTML("var allwtfacts = ".json_encode($facts).";\n");
		$out->addHTML("var allwtdetails = ".json_encode($details).";\n");
		$out->addHTML("var wtpagenotfound = ".($this->title->exists() ? 'false' : 'true').";\n");
		$out->addHTML("var wtallcategories = ['SampleCollection','Procedure','Workflow', 'ExecutedWorkflow', 'Task', 'Answer', 'AutomaticallyProvidedData', 'UserProvidedData', 'UserDescribedData', 'Component', 'Model', 'Person', 'Object', 'DataType', 'Thing'];\n");
		//$out->addHTML("var wtallcategories = ['UserProvidedData', 'UserDescribedData', 'Person', 'DataType', 'Thing'];\n");
		global $wgUseSimpleTasks, $wgDisableTracking;
		if(!$wgUseSimpleTasks) {
			$explorer = new WTTaskExplorer($this->title);
			$out->addHTML("var allwtexplorer = ".json_encode($explorer->getExplorer()).";\n");
		}
		$out->addHTML("var wtcategories = [];\n");
		$out->addHTML("var stdwtprops = [];\n"); 
		$cats = $out->getCategories();
		foreach($cats as $cat) {
			$out->addHTML("wtcategories['$cat'] = true;\n"); 
 			$props = WTFactsAPI::getCategoryProperties($cat); 
 			foreach($props as $prop) { 
 				$out->addHTML("stdwtprops['".$prop['label']."'] = ".json_encode($prop).";\n"); 
 			} 
		}
		global $wgUser;	 
 		$out->addHTML("var wtuid = null;\n"); 
 		if($wgUser->isLoggedIn()){ 
 			$uid = $wgUser->getName(); 
 			$out->addHTML("wtuid = '".$uid."';\n"); 
 		}  		
		$out->addHTML("var __use_simple_tasks = ".($wgUseSimpleTasks ? 'true' : 'false').";\n");
		$out->addHTML("var __disable_tracking = ".($wgDisableTracking ? 'true' : 'false').";\n");
		$out->addHTML("</script>\n"); 
	}

	function modifyWikiPage($out) {
		
		# -- Add the Standard Properties Table 
		//$this->addStdPropsDiv( $out ); 
				
		# -- Add a Facts Table
		$this->addFactsDiv( $out );

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

	function addTimelineDiv($out, $type) {
		$out->addHTML("<div id='main-timeline' class='props-table'></div>");
	}

	function addTaskMetaDataDiv($out) {
		$out->addHTML("<div id='main-taskmetadata'></div>");
	}
	
	function addFactsDiv($out) {
		$this->addStdPropsDiv($out);
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

	function addPersonDiv($out) {
		$out->addHTML("<div id='main-person' class='props-table'></div>");
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
