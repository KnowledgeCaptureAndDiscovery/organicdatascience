<?php

class WTData extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	/*function setJavascriptVariables($out) {
		parent::setJavascriptVariables($out);
		$out->addHTML("<script>var wtpagetype = 'WTData';</script>\n");
	}*/

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();

		# WTData Div
		$this->addDataDiv( $out );

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
		$details["WTData"] = $this->getWTData();
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

	function getWTData() {
		$details = array(
			'location'=>'', 
			'inputtoworkflows'=>array(),
			'generatedbyworkflows'=>array(),
			'types'=>array()
		);

		# SMW Initialization 
		$subject = SMWDIWikiPage::newFromTitle( $this->title );

		$locProp = SMWDIProperty::newFromUserLabel('DataHasLocation');
		$typeProp = SMWDIProperty::newFromUserLabel('DataHasType');
		//$pubbyProp = SMWDIProperty::newFromUserLabel('DataHasPubbyLink');
		$indataProp = SMWDIProperty::newFromUserLabel('WorkflowInputDataArtifact');
		$gendataProp = SMWDIProperty::newFromUserLabel('WorkflowDataArtifact');
	
		$location = smwfGetStore()->getPropertyValues( $subject, $locProp );
		$types = smwfGetStore()->getPropertyValues( $subject, $typeProp );
		//$pubbylinks = smwfGetStore()->getPropertyValues( $subject, $pubbyProp );

		$inexecs = smwfGetStore()->getPropertySubjects( $indataProp, $subject );
		$genexecs = smwfGetStore()->getPropertySubjects( $gendataProp, $subject );

		if(sizeof($location)>0)
			$details["location"] = $this->getUri($location[0]);
		foreach($types as $type) 
			$details['types'][] = $this->getUri($type);
		foreach($inexecs as $inexec) 
			$details['inputtoworkflows'][] = $this->getUri($inexec);
		foreach($genexecs as $genexec) 
			$details['generatedbyworkflows'][] = $this->getUri($genexec);
		//foreach($pubbylinks as $pubbylink) 
		//	$details['pubbylinks'][] = $this->getUri($pubbylink);
		return $details;
	}
}

