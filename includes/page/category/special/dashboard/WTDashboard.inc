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

		$html =  '<div id="dashboard"></div>Collaboration Graph';		
		$out->addHtml($html);		
	}
	
	
	/*TODO remove Hack!!!*/
	function includeJSHeaders($out) {
		global $wgDir;
		$out->addScriptFile( $wgDir . '/js/lib/sigma/sigma.min.js' );
		$out->addScriptFile( $wgDir . '/js/lib/sigma/plugins/sigma.parsers.json.min.js' );		
		$out->addScriptFile( $wgDir . '/js/lib/arbor/arbor.graphics.js' );
		$out->addScriptFile( $wgDir . '/js/lib/arbor/arbor.js' );
		$out->addScriptFile( $wgDir . '/js/lib/arbor/renderer.js' );
		$out->addScriptFile( $wgDir . '/js/page/category/special/WTDashboard.js' );
	}
	
	/*TODO remove Hack!!!*/
	function setJavascriptVariables($out) {
		$da = new WTDashboardAnalyze();
		$da->analyis();		
		$out->addHTML("<script>\n var dashboard = ".json_encode($da->toJson())."; \n</script>");
	}
}
