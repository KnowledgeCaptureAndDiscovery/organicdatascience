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
		global $wgDir;
		$out->addScriptFile('https://www.google.com/jsapi' );
		$out->addScriptFile( $wgDir . '/js/lib/sigma/sigma.min.js' );
		$out->addScriptFile( $wgDir . '/js/lib/sigma/plugins/sigma.parsers.json.min.js' );		
		$out->addScriptFile( $wgDir . '/js/lib/arbor/arbor.graphics.js' );
		$out->addScriptFile( $wgDir . '/js/lib/arbor/arbor.js' );
		$out->addScriptFile( $wgDir . '/js/lib/arbor/renderer.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/special/WTDashboard.js' );
	}
	
	/*TODO remove Hack for SpecialPage!!!*/
	function setJavascriptVariables($out) {
		$da = new WTDashboardAnalyze();
		$da->analyis();		
		$out->addHTML("<script>\n var dashboard = ".json_encode($da->toJson())."; \n</script>");
	}
}
