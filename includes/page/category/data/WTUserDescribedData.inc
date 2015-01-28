<?php

class WTUserDescribedData extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	/*function setJavascriptVariables($out) {
		parent::setJavascriptVariables($out);
		$out->addHTML("<script>var wtpagetype = 'WTUserDescribedData';</script>\n");
	}*/

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();

		# WTUserDescribedData Div
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
		$details["WTUserDescribedData"] = $this->getWTUserDescribedData();
		return $details;
	}

	protected function getUri($di) {
		if($di->getDIType() == SMWDataItem::TYPE_URI)
			return $di->getURI();
		else {
			$url = $di->getTitle();
			$url = preg_replace('/^http/i', 'http', $url);
			$url = preg_replace('/^ftp/i', 'ftp', $url);
			return $url;
		}
	}

	function getWTUserDescribedData() {
		$details = array(
			'location'=>''
		);

		# SMW Initialization 
		$subject = SMWDIWikiPage::newFromTitle( $this->title );

		$locProp = SMWDIProperty::newFromUserLabel('DataLocation');
		$locations = smwfGetStore()->getPropertyValues( $subject, $locProp );
		foreach($locations as $loc) {
			$details["location"] = $this->getUri($loc);
			break;
		}
		return $details;
	}
}

