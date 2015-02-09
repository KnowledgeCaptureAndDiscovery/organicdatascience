<?php

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
global $wgUseSimpleTasks, $wgDisableTracking;

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

$wgExtensionMessagesFiles['WorkflowTasks'] 	= $wgAbsDir . '/languages/WT_Messages.php';



$wgAPIModules['wtfacts'] = 'WTFactsAPI';
$wgAPIModules['wttasks'] = 'WTTaskAPI';
$wgAPIModules['wtadmin'] = 'WTAdminAPI';
$wgAPIModules['wtsuggest'] = 'WTSuggestAPI';

$wgSpecialPages['WTBatchTasks']  = 'WTBatchTasks';


$wgHooks['BeforePageDisplay'][] = 'WTRender';
function WTRender (&$out, &$skin) {
	global $wgRequest, $wgDir, $wgUseSimpleTasks;

	$title = $out->getTitle();
	$ns = $title->getNamespace();
	if (($ns !== NS_MAIN && $ns !== NS_USER) && ($ns !== SMW_NS_PROPERTY)) //SMW_NS_TYPE
		return false;

	$action = $wgRequest->getText( 'action' );
	if (($action !== 'view') && ($action !== 'purge') && ($action !== '')) 
		return false;

	$item = null;
	$cats = $out->getCategories();
	$url = $title->getPrefixedURL();

	if ($url === 'Main_Page') {
		$item = new WTMainPage($title);
	}
	else if ($ns === SMW_NS_PROPERTY) {
		$item = new WTProperty($title);
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
	else if(in_array("UserProvidedData", $cats)) {
		$item = new WTUserProvidedData($title);
	}
	else if(in_array("UserDescribedData", $cats)) {
		$item = new WTUserDescribedData($title);
	}
	else if(in_array("Component", $cats)) {
		$item = new WTComponent($title);
	}
	else if(in_array("Person", $cats)) {
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
		}
		return true;
	}
	else {
		$item = new WTBase($title);
	}

	$item->includeJSHeaders($out, $wgDir);
	$item->includeCSSHeaders($out, $wgDir);
	$item->setJavascriptVariables($out);
	$item->modifyWikiPage($out);

	return true;
}
