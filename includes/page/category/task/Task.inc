<?php

class Task extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	/*function setJavascriptVariables($out) {
		parent::setJavascriptVariables($out);
		$out->addHTML("<script>var wtpagetype = 'Task';</script>\n");
	}*/

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();		

		# Tree Div
		$this->addTreeDiv( $out, "Task" );
		
		# Timeline Div
		$this->addTimelineDiv( $out, "Task");
		
		# -- Add a Task Meta Data Table
		$this->addTaskMetaDataDiv( $out );

		# -- Add Existing Markup Here
		$out->addHTML( $old );

		# -- Add a Facts Table Here
		$this->addFactsDiv( $out );
		
//		# Answers Div
//		$this->addAnswersDiv( $out, "Answers" );

		# -- Credits Div
		$this->addCreditsDiv( $out );

		# -- Sidebar Div
		$this->addSidebarDiv( $out );
				
		$this->addYandexMetrica( $out );

		# -- CategoryChooser Div
		$this->addCategoryChooserDiv( $out );
	}


	function getPageDetails($title=null, $top=true, $just_contribs=false) {
		if(!$title) $title = $this->title;
		$subTasks = array();
		$answers = array();
		$answerDone = false;
	
		global $wgAnonCredit;
		$contributors = $this->getContributors($title, $wgAnonCredit);

		if($just_contribs) {
			uasort($contributors, "WTBase::compareContributors");
			return array("Contributors"=>$contributors);
		}

		$pagename = $title->getPrefixedText();
		$query = "[[$pagename]]";
		$rawparams = array($query, "?SubTask", "?Answer", "?Answered");
	
		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );
	
		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);
	
		$results = smwfGetStore()->getQueryResult( $queryobj );
		$res = $results->getNext();
	
		if($res && sizeof($res) == 4) {
			$subs = $res[1];
			$ans = $res[2];
			$done = $res[3];
			while($sub = $subs->getNextObject()) {
				$subtitle = $sub->getTitle();
				#$item = array("text"=>$subtitle->getText(), "url"=>$subtitle->getInternalURL(), "exists"=>$subtitle->exists);
				$item = array("text"=>$subtitle->getText(), "exists"=>$subtitle->exists(), "key"=>$subtitle->getDbKey());
				$subTasks[] = array("item"=>$item);
			}
			while($an = $ans->getNextObject()) {
				$antitle = $an->getTitle();
				#$item = array("text"=>$antitle->getText(), "url"=>$antitle->getInternalURL(), "exists"=>$antitle->exists);
				$item = array("text"=>$antitle->getText(), "exists"=>$antitle->exists(), "key"=>$antitle->getDbKey());
				$answers[] = array("item"=>$item);
			}
			$done = $done->getNextObject();
			if($done) $answerDone = true;
		}
	
		for($i=0; $i<sizeof($subTasks); $i++) {
			$sub = $subTasks[$i];
			$subdetails = $this->getPageDetails(Title::newFromText($sub["item"]["text"]), false);
			$subTasks[$i]["details"] = $subdetails;
			$contributors = $this->mergeContributors($contributors, $subdetails["Contributors"]);
		}
	
		for($i=0; $i<sizeof($answers); $i++) {
			$ans = $answers[$i];
			$ansdetails = $this->getPageDetails(Title::newFromText($ans["item"]["text"]), false, true);
			$answers[$i]["details"] = $ansdetails;
			$contributors = $this->mergeContributors($contributors, $ansdetails["Contributors"]);
		}
		
		$details = array(
				"SubTasks"=>$subTasks, 
				"Answers"=>$answers, 
				"Contributors"=>$contributors, 
				"Answered"=>$answerDone);

		if($top) {
			$details["item"] = array("text"=>$title->getText(), 
							"exists"=>$title->exists(),
							"key"=>$title->getDbKey());
			$details["Parents"] = $this->getParentTasks($title);
			uasort($details["Contributors"], "WTBase::compareContributors");
		}
		return $details;
	}

	function getParentTasks($title) {
		# SMW Initialization 
		$subject = SMWDIWikiPage::newFromTitle( $title );
		$subTask = new SMWDIProperty('SubTask');
	
		$parents = array();
		$parentQs = smwfGetStore()->getPropertySubjects( $subTask, $subject );
		foreach($parentQs as $parent) {
			$ptitle = $parent->getTitle();
			$item = array("text"=>$ptitle->getText(), "exists"=>$ptitle->exists(), "key"=>$ptitle->getDbKey());
			$parents[] = $item;
		}
		return $parents;
	}
}

