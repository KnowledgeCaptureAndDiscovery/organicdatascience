<?php
/**
 * API for adding and removing Task related SMW Property Values to a Title
 * 
 * @ingroup WorkflowTasks
 * @addtogroup API
 */
class WTTaskAPI extends ApiBase {

	public function __construct($query, $moduleName) {
		parent :: __construct($query, $moduleName);
	}

	public function execute() {
		$this->params = $this->extractRequestParams();
		$this->title = $this->getParam('title');
		$this->result = array();			
				
		switch ($this->getParam('operation')){
			case('add'): 			$this->add(); 			break;
			case('del'): 			$this->del(); 			break;
			case('move'): 			$this->move(); 			break;
			case('rename'): 		$this->rename(); 		break;
			case('toroot'): 		$this->toRoot(); 		break;			
			case('setmeta'): 		$this->setMetaFact(); 	break;
			case('addmeta'): 		$this->addMetaFact(); 	break;
			case('delmeta'): 		$this->delMetaFact(); 	break;
			case('getExplorer'): 	$this->getExplorer(); 	break;
		}			
			
		$exp = new WTTaskExplorer($this->title);
		$this->result['update'] = $exp->getExplorer();
		$this->getResult()->addValue(null, 'wttasks', $this->result);
	}
	
	private function getParam($param){
		$value = $this->params[$param];
		if (is_null($value))
			$this->dieUsage('Must specify '.$param, 'unspecified_'.$param);
		return $value;	
	}
	
	private function getBooleanParam($param){
		return $this->getParam($param) == "true";
	}
	
	private function hasAccessRight(){
		global $wgUser;
		if(!$wgUser->isLoggedIn()){
			$this->result['success'] = false;
			$this->result['error'] = 'Access denied!';
			return false;
		}
		return true;
	}
	
	//TODO history log should be changed?
	private function add(){	
		if(!$this->hasAccessRight())
			return;			
		$title = $this->getParam('title');	
		$subtitle = $this->getParam('subtitle'); 	
		$isRoot = $this->getBooleanParam('isroot');
        $subtitle = WTFactsAPI::replaceSpecialCharacters($subtitle);
        $exist = true;
		if(!WTTaskUtil::pageExist($subtitle) && !WTTaskUtil::taskExist($subtitle)){
			WTFactsAPI::createPageWithCategory($subtitle, 'Task');	
			if($isRoot){
				$owner = $this->getParam('owner');
				$expertise = $this->getParam('expertise');
				if($owner != WTTaskFacts::$unknown)
					WTFactsAPI::setFact($subtitle, 'Owner', $owner);			
				if($expertise != WTTaskFacts::$unknown)
					WTFactsAPI::setFact($subtitle, 'Expertise', $expertise);	
			}else
				WTFactsAPI::addFact($title, 'SubTask', $subtitle, null, true, null);			
			$exist = false;
		}	
		$this->result['exist'] = $exist;
		$this->result['title'] = $subtitle;
		$this->result['success'] = !$exist;
	}
	
	
	//TODO history log should be changed?
	private function del(){		
		if(!$this->hasAccessRight())
			return;
		$title = $this->getParam('title');			
		$taskId = $this->getParam('delId');			
		$parent = WTTaskUtil::findParenttaskByTaskid($taskId);
		if($parent != null)
			WTFactsAPI::delFact($parent, 'SubTask', $taskId, null, null);			
		$hasChildren = WTTaskUtil::findChildtasksByTaskid($taskId);
		$this->delCascade($taskId, $hasChildren, $title);		
		$this->result['success'] = true;
	}
	
	private function delCascade($taskId, &$hasChildren, $title){
		if(!$this->hasAccessRight())
			return;	
		/** If selected Page itself is deleted forward is needed */
		if($title == $taskId)
			$this->result['forward'] = true;
		if(array_key_exists($taskId, $hasChildren))
			foreach ($hasChildren[$taskId] as $childId)
				$this->delCascade($childId, $hasChildren, $title);
		WTFactsAPI::deleteWikiPage($taskId, 'Deletion of task '.$title);
	}
	
	//TODO history log should be changed?
	private function toRoot(){		
		if(!$this->hasAccessRight())
			return;		
		$sourceTask = $this->getParam('source');	
		if(!WTTaskUtil::pageExist($sourceTask))
			WTFactsAPI::createPageWithCategory($sourceTask, 'Task');	
		$sourceTaskParent = WTTaskUtil::findParenttaskByTaskid($sourceTask);
		if($sourceTaskParent != null)
			WTFactsAPI::delFact($sourceTaskParent, 'SubTask', $sourceTask, null, null);		
		$this->result['success'] = !$exist;
	}
	
	//TODO history log should be changed?
	private function move(){	
		if(!$this->hasAccessRight())
			return;					
		$sourceTask = $this->getParam('source');	
		$targetTask = $this->getParam('target');
		var_dump($sourceTask.$targetTask);
		$sourceTaskParent = WTTaskUtil::findParenttaskByTaskid($sourceTask);
		if($sourceTaskParent != null)
			WTFactsAPI::delFact($sourceTaskParent, 'SubTask', $sourceTask, null, null);
		if(!WTTaskUtil::pageExist($targetTask))
			WTFactsAPI::createPageWithCategory($targetTask, 'Task');	
		WTFactsAPI::addFact($targetTask, 'SubTask', $sourceTask, null, true, null);
		$this->result['success'] = true;
	}
	
	//TODO history log should be changed?
	private function rename(){	
		if(!$this->hasAccessRight())
			return;					
		$oldTitle = $this->getParam('oldtitle');	
		$newTitle = $this->getParam('newtitle');
		$newTitle = WTFactsAPI::replaceSpecialCharacters($newTitle);
		$ot = Title::newFromText($oldTitle);
		$nt = Title::newFromText($newTitle);
		$newTitle = $nt->mDbkeyform;	
		if($oldTitle == $newTitle){
			$this->result['success'] = true;			
		}else if(!WTTaskUtil::pageExist($newTitle) && !WTTaskUtil::taskExist($newTitle)) {
			if(!WTTaskUtil::pageExist($newTitle))
				$ot->moveTo($nt, true, "Moving page", false);
				//TODO grad user rights to prevent creating redirect page
			$parentTitle = WTTaskUtil::findParenttaskByTaskid($oldTitle);
			if($parentTitle != null){
				WTFactsAPI::delFact($parentTitle, 'SubTask', $oldTitle);
				WTFactsAPI::addFact($parentTitle, 'SubTask', $newTitle, null, true, null);
			}
			if($oldTitle == $this->getParam('title')){
				$this->result['forward'] = $newTitle;
				$this->title = $newTitle;
			}
			$this->result['success'] = true;
		}else{
			$this->result['success'] = false;	
			$this->result['exist'] = true;
		}		
	}
	
	private function setMetaFact(){
		if(!$this->hasAccessRight())
			return;
		$title = $this->getParam('title');	
		$property = $this->getParam('property');
		$value = $this->getParam('value');
		WTFactsAPI::setPropertyValue($title, $property, $value);
		if($property == 'Type' && ($value == 'medium' || $value == 'high'))
			WTFactsAPI::setPropertyValue($title, 'Progress', '');
	}
	
	private function addMetaFact(){
		if(!$this->hasAccessRight())
			return;
		$title = $this->getParam('title');	
		$property = $this->getParam('property');
		$value = $this->getParam('value');
		WTFactsAPI::addFact($title, $property, $value, null, true, null);
	}
	
	private function delMetaFact(){
		if(!$this->hasAccessRight())
			return;
		$title = $this->getParam('title');	
		$property = $this->getParam('property');
		$value = $this->getParam('value');
		WTFactsAPI::delFact($title, $property, $value, null, null);
	}

	private function getExplorer(){
		$title = $this->getParam('title');	
		$exp = new WTTaskExplorer($title);
		$this->result['result'] = $exp->getExplorer();	
	}
	
	
	protected function getAllowedParams() {
		return array (
			'operation' => null,
			'title' => null,
			'subtitle' => null,
			'property' => null,
			'value' => null,
			'source' => null,
			'sourceparent' => null,
			'target' => null,
			'selectedId' => null,
			'delId' => null,
			'oldtitle' => null,
			'newtitle' => null,
			'isroot' => null,
			'owner' => null,
			'expertise' => null
		);
	}

	protected function getParamDescription() {
		return array (
			'operation' => 'Operation to be performed. One of show, add, del, newpage, updatesubobjects',
			'title' => 'Title of the page (required)',
			'property' => 'Property of the page (required for add, del)',
			'value' => 'Value of the page Property (required for add)',
			'type' => 'Type of the Property value. One of page, number, boolean (optional for add)',
			'subtitle' => 'Category of the new page (required for newpage)',
			'souce' => 'Source task of move operation',
			'sourceparent' => 'Source parent task',
			'target' => 'Target task of move operation',
			'selectedId' => 'Selected Task id need for updating data',
			'delId' => 'Task id to delete',
			'oldtitle' => 'Original title to rename',
			'newtitle' => 'Rename to new title',
			'isroot' => 'task is on root level',
			'owner' => 'task owner',
			'expertise' => 'task experise'
		);
	}

	protected function getDescription() {
		return 'Shows, adds or removes task related semantic facts for a page';
	}

	protected function getExamples() {
		return array (
			'api.php?action=wtfacts&operation=show&title=TestSMW&format=json'
		);
	}

	public function getVersion() {
		return __CLASS__ . ': $Id$';
	}
}
