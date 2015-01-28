<?php

class Answer extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	/*function setJavascriptVariables($out) {
		parent::setJavascriptVariables($out);
		$out->addHTML("<script>var wtpagetype = 'Answer';</script>\n");
	}*/

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();

		# Tasks Div
		$this->addTasksDiv( $out );

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
		$details["Tasks"] = $this->getTasks();
		return $details;
	}

	function getTasks() {
		# SMW Initialization 
		$object = SMWDIWikiPage::newFromTitle( $this->title );
		$ansProp = SMWDIProperty::newFromUserLabel('Answer');
	
		$tasks = array();
		$parentQs = smwfGetStore()->getPropertySubjects( $ansProp, $object );
		foreach($parentQs as $parent) {
			$ptitle = $parent->getTitle();
			$item = array("text"=>$ptitle->getText(), "exists"=>$ptitle->exists(), "key"=>$ptitle->getDbKey());
			$tasks[] = $item;
		}
		return $tasks;
	}
}

