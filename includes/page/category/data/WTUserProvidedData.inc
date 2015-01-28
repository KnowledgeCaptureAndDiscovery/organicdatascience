<?php

class WTUserProvidedData extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	/*function setJavascriptVariables($out) {
		parent::setJavascriptVariables($out);
		$out->addHTML("<script>var wtpagetype = 'WTUserProvidedData';</script>\n");
	}*/

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();

		# WTUserProvidedData Div
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
		$details["WTUserProvidedData"] = $this->getWTUserProvidedData();
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

	function getWTUserProvidedData() {
		$details = array(
			'location'=>'',
			'extractedFrom'=>array()
		);

		# SMW Initialization 
		$subject = SMWDIWikiPage::newFromTitle( $this->title );
		$locProp = SMWDIProperty::newFromUserLabel('DataWikiLocation');
		$extProp = SMWDIProperty::newFromUserLabel('DataExtractedFrom');

		$locations = smwfGetStore()->getPropertyValues( $subject, $locProp );
		foreach($locations as $loc) {
			$details["location"] = $this->getUri($loc);
			break;
		}
		$extFroms = smwfGetStore()->getPropertyValues( $subject, $extProp );
		foreach($extFroms as $ext) {
			$details["extractedFrom"][] = $this->getUri($ext);
		}
		return $details;
	}
}

