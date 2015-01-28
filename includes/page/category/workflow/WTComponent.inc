<?php

class WTComponent extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	/*function setJavascriptVariables($out) {
		parent::setJavascriptVariables($out);
		$out->addHTML("<script>var wtpagetype = 'WTComponent';</script>\n");
	}*/

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();

		# WTComponent Div
		$this->addComponentDiv( $out );

		# -- Add Existing Markup Here
		$out->addHTML( $old );

		# -- Add a Facts Table Here
		$this->addFactsDiv( $out );

		# -- Credits Div
		$this->addCreditsDiv( $out );

		# -- CategoryChooser Div
		$this->addCategoryChooserDiv( $out );
	}

	function getPageDetails() {
		$details = parent::getPageDetails();
		$details["WTComponent"] = $this->getWTComponent();
		return $details;
	}

	protected function getUri($di) {
		if($di->getDIType() == SMWDataItem::TYPE_URI)
			return $di->getURI();
		else {
			$url = $di->getTitle();
			return preg_replace('/^http/i', 'http', $url);
		}
	}

	function getWTComponent() {
		$details = array(
			'inputdatavars'=>array(),
			'inputparamvars'=>array(),
			'outputdatavars'=>array(),
			'usedintemplates'=>array()
		);

		# SMW Initialization 
		$subject = SMWDIWikiPage::newFromTitle( $this->title );

		$idvProp = SMWDIProperty::newFromUserLabel('InputDataVariable');
		$ipvProp = SMWDIProperty::newFromUserLabel('InputParamVariable');
		$odvProp = SMWDIProperty::newFromUserLabel('OutputDataVariable');
		$wcompProp = SMWDIProperty::newFromUserLabel('WorkflowProcess');
	
		$idvs = smwfGetStore()->getPropertyValues( $subject, $idvProp );
		$ipvs = smwfGetStore()->getPropertyValues( $subject, $ipvProp );
		$odvs = smwfGetStore()->getPropertyValues( $subject, $odvProp );
		$inwflows = smwfGetStore()->getPropertySubjects( $wcompProp, $subject );

		foreach($idvs as $idv) 
			$details['inputdatavars'][] = $this->getUri($idv);
		foreach($ipvs as $ipv) 
			$details['inputparamvars'][] = $this->getUri($ipv);
		foreach($odvs as $outdata) 
			$details['outputdatavars'][] = $this->getUri($outdata);
		foreach($inwflows as $inwflow) 
			$details['usedintemplates'][] = $this->getUri($inwflow);
		return $details;
	}
}

