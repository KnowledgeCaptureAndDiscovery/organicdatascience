<?php
/**
 * API for adding new user tutorials and updating the documenation.
 * 
 * @ingroup WorkflowTasks
 * @addtogroup API
 */
class WTAdminAPI extends ApiBase {

	public function __construct($query, $moduleName) {
		parent :: __construct($query, $moduleName);
	}

	public function execute() {
		$this->params = $this->extractRequestParams();
		$this->result = array();			
				
		switch ($this->getParam('operation')){
			case('addtraining'): 	$this->addTraining(); 	break;
			case('updatedocu'): 	$this->updateDocu(); 	break;
		}			
		$this->getResult()->addValue(null, 'wtadmin', $this->result);
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
		if(!$wgUser->isLoggedIn() || $wgUser->getName() != 'Admin'){
			$this->result['success'] = false;
			$this->result['error'] = 'Access denied!';
			return false;
		}
		return true;
	}
	
	private function addTraining(){	
		if(!$this->hasAccessRight())
			return;			
		$username = $this->getParam('username');	
		$fullname = $this->getParam('fullname');
		new WTTraining($fullname, $username);   
		$this->result['success'] = true;
	}
	
	private function updateDocu(){
		if(!$this->hasAccessRight())
			return;		
		WTDocumentation::create();
		$this->result['success'] = true;
	}

	
	protected function getAllowedParams() {
		return array (
			'operation' => null,
			'username' => null,
			'fullname' => null
		);
	}

	protected function getParamDescription() {
		return array (
			'operation' => 'Operation to be performed. One of addtraining, updatedocu',
			'username' => 'Username is similar with the user id',
			'fullname' => 'The users full name - "Firstname Lastname"'
		);
	}

	protected function getDescription() {
		return 'Creating new user Tutorials and updating the docu!';
	}

	protected function getExamples() {
		return array (
			'api.php?action=wtadmin&operation=show&title=TestSMW&format=json'
		);
	}

	public function getVersion() {
		return __CLASS__ . ': $Id$';
	}
}
