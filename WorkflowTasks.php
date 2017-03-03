<?php
use SMW\ContentParser;

$wgExtensionCredits['validextensionclass'][] = array(
	'path' => __FILE__,
	'name' => 'WorkflowTasks',
	'author' =>'Varun Ratnakar', 
	'url' => 'http://www.isi.edu/~varunr/wiki/WorkflowTasks', 
	'description' => 'Uses Semantic Media Wiki. Handles Categories: Task, Answer, Workflow, ExecutedWorkflow, Data, Component',
	'version'  => 0.1,
	);

global $wgDir;
global $wgScriptPath;
$wgAbsDir = dirname(__File__);
$wgDir = $wgScriptPath."/extensions/WorkflowTasks";

# Global variables that can be defined in LocalSettings.php
global $wgUseSimpleTasks, $wgDisableTracking, $wgUseLiPD;

$wgAutoloadClasses['WTBase'] 				= $wgAbsDir . '/includes/page/WTBase.inc';
$wgAutoloadClasses['WTMainPage'] 			= $wgAbsDir . '/includes/page/category/main/WTMainPage.inc';
$wgAutoloadClasses['Task'] 					= $wgAbsDir . '/includes/page/category/task/Task.inc';
$wgAutoloadClasses['SimpleTask']			= $wgAbsDir . '/includes/page/category/task/SimpleTask.inc';
$wgAutoloadClasses['Answer'] 				= $wgAbsDir . '/includes/page/category/answer/Answer.inc';
$wgAutoloadClasses['Workflow'] 				= $wgAbsDir . '/includes/page/category/workflow/Workflow.inc';
$wgAutoloadClasses['ExecutedWorkflow'] 		= $wgAbsDir . '/includes/page/category/workflow/ExecutedWorkflow.inc';
$wgAutoloadClasses['WTComponent'] 			= $wgAbsDir . '/includes/page/category/workflow/WTComponent.inc';
$wgAutoloadClasses['WTData'] 				= $wgAbsDir . '/includes/page/category/data/WTData.inc';
$wgAutoloadClasses['WTUserProvidedData'] 	= $wgAbsDir . '/includes/page/category/data/WTUserProvidedData.inc';
$wgAutoloadClasses['WTUserDescribedData'] 	= $wgAbsDir . '/includes/page/category/data/WTUserDescribedData.inc';
$wgAutoloadClasses['WTPerson'] 				= $wgAbsDir . '/includes/page/category/person/WTPerson.inc';
$wgAutoloadClasses['WTNewPerson'] 			= $wgAbsDir . '/includes/page/category/person/WTNewPerson.inc';
$wgAutoloadClasses['WTWorkingGroup'] 		= $wgAbsDir . '/includes/page/category/workgroup/WTWorkingGroup.inc';
$wgAutoloadClasses['WTProperty'] 			= $wgAbsDir . '/includes/page/component/WTProperty.inc';

$wgAutoloadClasses['WTFactsAPI'] 			= $wgAbsDir . '/includes/core/api/WTFactsAPI.inc';
$wgAutoloadClasses['WTTaskAPI'] 			= $wgAbsDir . '/includes/core/api/WTTaskAPI.inc';
$wgAutoloadClasses['WTAdminAPI'] 			= $wgAbsDir . '/includes/core/api/WTAdminAPI.inc';
$wgAutoloadClasses['WTSuggestAPI'] 			= $wgAbsDir . '/includes/core/api/WTSuggestAPI.inc';
$wgAutoloadClasses['WTTaskUtil'] 			= $wgAbsDir . '/includes/core/util/WTTaskUtil.inc';
$wgAutoloadClasses['WTTaskFacts'] 			= $wgAbsDir . '/includes/core/util/WTTaskFacts.inc';
$wgAutoloadClasses['WTTaskExplorer'] 		= $wgAbsDir . '/includes/core/util/WTTaskExplorer.inc';
$wgAutoloadClasses['WTDocumentation'] 		= $wgAbsDir . '/includes/core/init/docu/WTDocumentation.inc';
$wgAutoloadClasses['WTTraining'] 			= $wgAbsDir . '/includes/core/init/training/WTTraining.inc';
$wgAutoloadClasses['WTSample'] 				= $wgAbsDir . '/includes/core/init/sample/WTSample.inc';

$wgAutoloadClasses['WTDocu'] 				= $wgAbsDir . '/includes/page/category/special/WTDocu.inc';
$wgAutoloadClasses['WTAdmin'] 				= $wgAbsDir . '/includes/page/category/special/WTAdmin.inc';
$wgAutoloadClasses['WTBatchTasks'] 			= $wgAbsDir . '/includes/page/category/special/WTBatchTasks.inc';

$wgAutoloadClasses['WTLiPD'] 				= $wgAbsDir . '/includes/page/category/special/lipd/WTLiPD.inc';
$wgAutoloadClasses['WTBootstrapLE'] 		= $wgAbsDir . '/includes/page/category/special/lipd/WTBootstrapLE.inc';
$wgAutoloadClasses['WTDataset'] 				= $wgAbsDir . '/includes/page/category/special/lipd/WTDataset.inc';

$wgAutoloadClasses['WTDashboard'] 			= $wgAbsDir . '/includes/page/category/special/dashboard/WTDashboard.inc';
$wgAutoloadClasses['WTDashboardAnalyze'] 	= $wgAbsDir . '/includes/page/category/special/dashboard/WTDashboardAnalyze.inc';
$wgAutoloadClasses['WTCollaborationGraph'] 	= $wgAbsDir . '/includes/page/category/special/dashboard/WTCollaborationGraph.inc';
$wgAutoloadClasses['WTAncestorDigram'] 		= $wgAbsDir . '/includes/page/category/special/dashboard/WTAncestorDigram.inc';
$wgAutoloadClasses['WTChildrenDigram'] 		= $wgAbsDir . '/includes/page/category/special/dashboard/WTChildrenDigram.inc';
$wgAutoloadClasses['WTSummary'] 			= $wgAbsDir . '/includes/page/category/special/dashboard/WTSummary.inc';

$wgExtensionMessagesFiles['WorkflowTasks'] 	= $wgAbsDir . '/languages/WT_Messages.php';

$wgResourceModules['WorkflowTasks.extra'] = array(
	'position' => 'top',
	'scripts' => array(
		'js/lib/jquery/noty/jquery.noty.min.js',
		'js/lib/jquery/noty/top.min.js',
		'js/lib/jquery/noty/relax.min.js',
		'js/lib/jquery/select2/select2.js',
		'js/lib/jquery/jquery.loadmask.min.js',
		'js/lib/jquery/jquery.autocomplete2.js',
		'js/lib/jquery/jquery.tooltipster.min.js',
		'js/lib/jquery/jquery.picker.js',
		'js/lib/jquery/jquery.picker.date.js',
		'js/lib/jquery/jquery.lightbox_me.js',
		'js/lib/moment/moment.min.js',
		'js/lib/jstree/jstree.js',
		'js/lib/raphael/raphael-min.js',
	),
	'styles' => array(
		'css/font-awesome/css/font-awesome.min.css',
		'css/academicons/css/academicons.min.css',
		'css/jquery/jquery.loadmask.css',		
		'css/jquery/jquery.autocomplete.css',
		'css/jquery/jquery.tooltipster.css',
		'css/jquery/jquery.tooltipster.custom.css',
		'css/jquery/jquery.picker.classic.css',
		'css/jquery/jquery.picker.classic.date.css',
		'css/jquery/jquery.picker.classic.date.custom.css',
		'css/select2/select2.min.css',
		'css/jstree/style.css',
		'css/jstree/style.custom.css'
	),
	'dependencies' => array(
		'jquery.ui.core',
		'jquery.ui.autocomplete',
		'jquery.ui.widget',
		'jquery.ui.dialog',
	),
	'localBasePath' => __DIR__,
	'remoteExtPath' => 'WorkflowTasks'
);

$wgResourceModules['WorkflowTasks'] = array(
	'position' => 'top',
	'scripts' => array(
		'includes/tracking/WTTracker.js',
		'js/core/api/WTAPI.js',
		'js/core/api/WTExplorerParser.js',
		'js/core/api/WTExplorerAPI.js',
		'js/core/util/WTUtil.js',
		'js/core/ui/WTSidebar.js',
		'js/core/ui/WTExplorerMenu.js',
		'js/core/ui/WTExplorerEvents.js',
		'js/core/ui/WTSidebarResizer.js',
		'js/core/ui/WTTimeline.js',
		'js/core/ui/WTTaskAlert.js',
		'js/core/ui/WTDialog.js',
		'js/page/component/WTFacts.js',
		'js/page/component/WTStdProperties.js',
		'js/page/component/WTInProperties.js',
		'js/page/component/WTCredits.js',
		'js/page/component/WTCategoryChooser.js',
		'js/page/component/WTDataColumns.js',
		'js/page/category/task/WTTasks.js',
		'js/page/category/task/WTTaskContext.js',
		'js/page/category/task/WTSubTasks.js',
		'js/page/category/task/WTSimpleTasks.js',
		'js/page/category/task/WTTaskMetaData.js',
		'js/page/category/answer/WTAnswers.js',
		'js/page/category/data/WTData.js',
		'js/page/category/data/WTUserProvidedData.js',
		'js/page/category/data/WTUserDescribedData.js',
		'js/page/category/workflow/WTComponent.js',
		'js/page/category/workflow/WTWorkflow.js',
		'js/page/category/workflow/WTExecutedWorkflow.js',
		'js/page/category/person/WTPerson.js',
		'js/page/category/person/WTNewPerson.js',
		'js/page/category/person/WTPersonExpertise.js',
		'js/page/category/person/WTPersonContext.js',
		'js/page/category/person/WTPersonTasks.js',
		'js/page/category/special/WTDocu.js',
		'js/page/category/special/WTAdmin.js',
		'js/main.js'
	),
	'dependencies' => array(
		'WorkflowTasks.extra'
	),
	'localBasePath' => __DIR__,
	'remoteExtPath' => 'WorkflowTasks'
);

$wgResourceLoaderDebug = false;
$wgResourceModules['WorkflowTasks.dashboard'] = array(
    'position' => 'bottom',
    'scripts' => array(
		'js/lib/google/jsapi.js',
		'js/lib/sigma/sigma.min.js',
		'js/lib/sigma/plugins/sigma.parsers.json.min.js',
		'js/lib/arbor/arbor.js',
		'js/lib/arbor/arbor.graphics.js',
		'js/lib/arbor/renderer.js',
		'js/page/category/special/WTDashboard.js'
	),
	'localBasePath' => __DIR__,
	'remoteExtPath' => 'WorkflowTasks'
);


$wgAPIModules['wtfacts'] = 'WTFactsAPI';
$wgAPIModules['wttasks'] = 'WTTaskAPI';
$wgAPIModules['wtadmin'] = 'WTAdminAPI';
$wgAPIModules['wtsuggest'] = 'WTSuggestAPI';

$wgSpecialPages['WTBatchTasks']  = 'WTBatchTasks';
$wgSpecialPages['WTDashboard']  = 'WTDashboard';
$wgSpecialPageGroups['WTBatchTasks']  = 'ODSGroup';
$wgSpecialPageGroups['WTDashboard']  = 'ODSGroup';

$wgSpecialPages['WTLiPD']  = 'WTLiPD';
$wgSpecialPageGroups['WTLiPD']  = 'ODSGroup';
$wgSpecialPages['WTBootstrapLE']  = 'WTBootstrapLE';
$wgSpecialPageGroups['WTBootstrapLE']  = 'ODSGroup';

$wgHooks['BeforePageDisplay'][] = 'WTRender';

global $wgCore;
$wgCore = "©";

function WTRender (&$out, &$skin) {
	global $wgRequest, $wgDir, $wgUseSimpleTasks, $wgCore;

	$item = null;
	$title = $out->getTitle();
	$url = $title->getPrefixedURL();
	$ns = $title->getNamespace();
	$cats = $out->getCategories();

	$action = $wgRequest->getText( 'action' );

	if ( (($ns !== NS_MAIN) && ($ns !== NS_USER) && ($ns !== SMW_NS_PROPERTY) && ($ns != NS_CATEGORY)) 
			|| (($action !== 'view') && ($action !== 'purge') && ($action !== '')) ) {
		$item = new WTBase($title);
		$item->addPageHeader($out);
		$item->includeCSSHeaders($out, $wgDir);
		$item->addCategoryHeaders($out, $cats);
		return false;
	}

	if ($url === 'Main_Page') {
		$item = new WTMainPage($title);
	}
	else if ($ns === SMW_NS_PROPERTY) {
		$item = new WTProperty($title);
	}
	else if ($ns === NS_CATEGORY) { 
		if(WTWorkingGroup::isWorkingGroup($title->getDbKey())) 
			$item = new WTWorkingGroup($title);
		else {
			$item = new WTBase($title);
			$item->addPageHeader($out);
			$item->includeCSSHeaders($out, $wgDir);
			$item->includeJSHeaders($out);
			$item->addCategoryHeaders($out, $cats);
			$item->setJavascriptVariables($out, true);
			$out->addHTML("<br />");
			$item->addCreditsDiv($out);
			return false;
		}
	}
	else if(in_array("Task", $cats)) {
		if($wgUseSimpleTasks)
			$item = new SimpleTask($title);
		else
			$item = new Task($title);
	}
	else if(in_array("Procedure", $cats)) {
		$item = new SimpleTask($title);
	}
	else if(in_array("Answer", $cats)) {
		$item = new Answer($title);
	}
	else if(in_array("Workflow", $cats)) {
		$item = new Workflow($title);
	}
	else if(in_array("ExecutedWorkflow", $cats)) {
		$item = new ExecutedWorkflow($title);
	}
	else if(in_array("AutomaticallyProvidedData", $cats)) {
		$item = new WTData($title);
	}
	else if(in_array("DataTable $wgCore", $cats)) {
		$item = new WTUserProvidedData($title, "HasFileName $wgCore");
	}
	else if(in_array("Dataset $wgCore", $cats)) {
		$item = new WTDataset($title);
	}
	else if(in_array("UserProvidedData", $cats)) {
		$item = new WTUserProvidedData($title);
	}
	else if(in_array("UserDescribedData", $cats)) {
		$item = new WTUserDescribedData($title);
	}
	else if(in_array("Component", $cats)) {
		$item = new WTComponent($title);
	}
	else if(in_array("Person", $cats) || in_array("Person $wgCore", $cats)) {
		$item = new WTPerson($title);
	}
	else if(in_array("Docu", $cats)) {
		$item = new WTDocu($title);
	}
	else if(in_array("Admin", $cats)) {
		$item = new WTAdmin($title);
	}
	else if($ns == NS_USER) {
		$uname = WTPerson::getPersonNameFromUserId($title->getDbKey());
		if($uname) {
			$title = Title::newFromText($uname);
			header('Location: '. $title->getFullUrl());
			return false;
		}
		else {
			$item = new WTNewPerson($title);
		}
	}
	else {
		$item = new WTBase($title);
	}

	$item->addPageHeader($out);
	$item->includeJSHeaders($out, $wgDir);
	$item->includeCSSHeaders($out, $wgDir);
	$item->setJavascriptVariables($out);
	$item->modifyWikiPage($out);
	$item->addCategoryHeaders($out, $cats);

	return true;
}


/*$wgHooks['SidebarBeforeOutput'][] = 'WTSidebar';
function WTSidebar ($skin, &$sidebar) {
	$main = new WTMainPage("Main_Page");
	$main->modifySidebar($sidebar);
	return true;
}*/

$wgAvailableRights[] = "edit-assertions";
$wgAvailableRights[] = "edit-properties";

function diffSemanticData($curdata, $newdata, &$summary) {
	$diff = array("added"=>[], "modified"=>[], "deleted"=>[]);

	$curprops = [];
	$newprops = [];
	if($curdata) {
		foreach ( $curdata->getProperties() as $curprop ) 
			$curprops[$curprop->getKey()] = $curprop;
	}
	if($newdata) {
		foreach ( $newdata->getProperties() as $newprop ) 
			$newprops[$newprop->getKey()] = $newprop;
	}

	foreach ( $newprops as $pkey=>$newprop ) {
		if($pkey == "_MDAT" || $pkey == "_SKEY")
			continue;

		if(!array_key_exists($pkey, $curprops)) {
			// Added
			$diff["added"][] = $pkey;
		}
		else {
			// Modified
			$newvals = [];
			$curvals = [];
			foreach ( array_unique($newdata->getPropertyValues($newprop), SORT_REGULAR) as $dataItem ) 
				$newvals[] = WTFactsAPI::getDIText($dataItem);
			foreach ( array_unique($curdata->getPropertyValues($newprop), SORT_REGULAR) as $dataItem ) 
				$curvals[] = WTFactsAPI::getDIText($dataItem);

			$diff1 = array_diff($newvals, $curvals);
			$diff2 = array_diff($curvals, $newvals);
			if(sizeof($diff1)>0 || sizeof($diff2)>0)
				$diff["modified"][] = $pkey;
		}
	}

	foreach ( $curprops as $pkey=>$curprop ) {
		// Deleted
		if($pkey == "_MDAT" || $pkey == "_SKEY")
			continue;
		if(!array_key_exists($pkey, $newprops)) {
			// Deleted
			$diff["deleted"][] = $pkey;
		}
	}
	return $diff;
}

function getSemanticChanges($wikiPage, $newcontent, &$summary) {
	$contentParser = new ContentParser( $wikiPage->getTitle() );
	$parsed = $contentParser->parse($newcontent->serialize());
	$newdata = $parsed->getOutput()->getExtensionData("smwdata");
	$curcontent = $wikiPage->getContent();
	$curparsed = $contentParser->parse($curcontent ? $curcontent->serialize() : "");
	$curdata = $curparsed->getOutput()->getExtensionData("smwdata");
	//$subject = SMWDIWikiPage::newFromTitle( $wikiPage->getTitle() );
	//$curdata = smwfGetStore()->getSemanticData($subject);
	return diffSemanticData($curdata, $newdata, $summary);
}

function getPageCreator($wikiPage) {
	$dbr = wfGetDB( DB_SLAVE );
	$revision = $dbr->tableName( 'revision' );
	$pageid = $wikiPage->getId();
	$query = "SELECT rev_user_text FROM ".$revision." WHERE rev_page=".$pageid." ORDER BY rev_timestamp ASC LIMIT 1";
	$res = $dbr->query($query);
	if( $res && $dbr->numRows( $res ) > 0 ) {
		$row = $dbr->fetchObject( $res );
		return $row->rev_user_text;
	}
	return null;
}

$wgHooks['PageContentSave'][] = 'checkUserPermissions';
function checkUserPermissions( &$wikiPage, &$user, &$content, &$summary, 
		$isMinor, $isWatch, $section, &$flags, &$status ) { 
	global $wgCore;

	$ns = $wikiPage->getTitle()->getNamespace();
	$title = $wikiPage->getTitle()->getText();
	$isCore = (preg_match("/$wgCore$/", $title)) || (preg_match("/^MediaWiki:Smw_import_core/", $title));
	$isOntPage = ($ns == 14 || $ns == 102);
	if(!$isOntPage && $ns != 0)
		return;

	$numSemanticChanges = 0;
	$diff = getSemanticChanges($wikiPage, $content, $summary);
	$numSemanticChanges = sizeof($diff["modified"]) + sizeof($diff["added"]) + sizeof($diff["deleted"]);

	$isCreator = true;
	$isNew = !$wikiPage->getTitle()->exists();
	if(!$isNew) {
		$creator = getPageCreator($wikiPage);
		$isCreator = ($creator == $user->getName());
	}

	$rightNeeded = "edit";
	if(!$isOntPage) { // Normal page
		if($numSemanticChanges==0) // Textual changes
			$rightNeeded = "edit-page-text";
		else // Semantic changes
			$rightNeeded = "edit-page-metadata";
	}
	else { // Ontology page
		if($numSemanticChanges==0) // Textual changes
			$rightNeeded = "edit-ontology-text";
		else // Semantic changes
			$rightNeeded = "edit-ontology-semantics";
	}

	$errorMessage = null;
	$rightNeededCreator = "$rightNeeded-creator";
	if(!$user->isAllowed($rightNeeded)) {
		if($user->isAllowed($rightNeededCreator) && !$isCreator) 
			$errorMessage = wfMessage("right-$rightNeeded")->text(). " created by someone else";
		else
			$errorMessage = wfMessage("right-$rightNeeded")->text();
	}
	else if($isCore && $isOntPage && !$user->isAllowed("edit-core-ontology"))
		$errorMessage = wfMessage("right-edit-core-ontology")->text();

	if($errorMessage) {
		$message = "No permission to ". lcfirst($errorMessage);
		$status->fatal(new RawMessage($message));
	}
}

function getRevisionCategories($title, $rev, $parseTimestamp ) {
	$content = $rev->getContent();
	$options = $content->getContentHandler()->makeParserOptions( 'canonical' );
	$options->setTimestamp( $parseTimestamp );
	$output = $content->getParserOutput( $title, $rev->getId(), $options );
	return array_map( 'strval', array_keys( $output->getCategories() ) );
}

$wgHooks['PageContentSaveComplete'][] = 'updateWorkingGroupsWatchlist';
function updateWorkingGroupsWatchlist( &$article, &$user, $content, $summary, 
		$isMinor, $isWatch, $section, &$flags, $revision, &$status ) { 
	if($revision == null)
		return;
	$prevision = $revision->getPrevious();
	$ts = $revision->getTimestamp();
	$newcats = getRevisionCategories($article->getTitle(), $revision, $ts);
	$oldcats = $prevision ? getRevisionCategories($article->getTitle(), $prevision, $ts) : [];
	
	$added_cats = array_diff($newcats, $oldcats);
	$removed_cats = array_diff($oldcats, $newcats);

	foreach($added_cats as $cat) {
		if(WTWorkingGroup::isWorkingGroup($cat)) {
			$members = WTWorkingGroup::getMembersOfWorkingGroup($cat);
			foreach($members as $member) {
				WTPerson::watchPage($member, $article->getTitle());
			}
		}
	}
	foreach($removed_cats as $cat) {
		if(WTWorkingGroup::isWorkingGroup($cat)) {
			$members = WTWorkingGroup::getMembersOfWorkingGroup($cat);
			foreach($members as $member) {
				WTPerson::unwatchPage($member, $article->getTitle());
			}
		}
	}
}

