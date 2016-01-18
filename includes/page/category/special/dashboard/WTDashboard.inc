<?php

class WTDashboard extends SpecialPage {
	
	public function __construct() {
		parent::__construct( 'WTDashboard' );		
	}
	
	function execute( $par ) {		
		$request = $this->getRequest();
		$out = $this->getOutput();		
		$this->setHeaders();
		$this->includeJSHeaders($out);
		$this->setJavascriptVariables($out);
		$out->addHtml('<div id="dashboard"></div>');		
	}	
	
	/*TODO remove Hack for SpecialPage!!!*/
	function includeJSHeaders($out) {
		$out->addModules('WorkflowTasks.dashboard');
	}
	
	/*TODO remove Hack for SpecialPage!!!*/
	function setJavascriptVariables($out) {
		$da = new WTDashboardAnalyze();
		$da->analyis();		
		$out->addHTML("<script>\n var dashboard = ".json_encode($da->toJson())."; \n</script>");
	}

	function getGroupName() {
		return "ODSGroup";
	}
}
