<?php

class Workflow extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	/*function setJavascriptVariables($out) {
		parent::setJavascriptVariables($out);
		$out->addHTML("<script>var wtpagetype = 'Workflow';</script>\n");
	}*/

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();

		# Workflow Div
		$this->addWorkflowDiv( $out );

		# -- Add Existing Markup Here
		$out->addHTML( $old );

		# -- Add a Facts Table Here
		$this->addFactsDiv( $out );

		# -- Credits Div
		$this->addCreditsDiv( $out );

		# -- CategoryChooser Div
		$this->addCategoryChooserDiv( $out );
	}

	public function getPageDetails($title=null) {
		$details = parent::getPageDetails();
		$details["Workflow"] = $this->getWorkflow();
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

	function getWorkflow() {
		$details = array('url'=>'', 
			'contributor'=>'', 
			'wingstemplate'=>'', 
			'templateimage'=>'', 
			'createdin'=>'', 
			'variables'=>array(),
			'processes'=>array(),
			'executions'=>array()
		);

		# SMW Initialization 
		$subject = SMWDIWikiPage::newFromTitle( $this->title );
		$store = &smwfGetStore();

		$url = "";
		$wflowProp = SMWDIProperty::newFromUserLabel('Workflow');
		$wflows = $store->getPropertyValues( $subject, $wflowProp );
		foreach($wflows as $wflow) {
			$url = $this->getUri($wflow);
			break;
		}

		if($url) {
			$dvarProp = SMWDIProperty::newFromUserLabel('WorkflowDataVariable');
			$pvarProp = SMWDIProperty::newFromUserLabel('WorkflowParamVariable');
			$procProp = SMWDIProperty::newFromUserLabel('WorkflowProcessVariable');
			$contProp = SMWDIProperty::newFromUserLabel('WorkflowContributor');
			$timageProp = SMWDIProperty::newFromUserLabel('WorkflowTemplateImage');
			$tfileProp = SMWDIProperty::newFromUserLabel('WorkflowWingsTemplate');
			$crinProp = SMWDIProperty::newFromUserLabel('WorkflowCreatedIn');
			$wtempProp = SMWDIProperty::newFromUserLabel('WorkflowTemplate');
	
			$dvars = $store->getPropertyValues( $subject, $dvarProp );
			$pvars = $store->getPropertyValues( $subject, $pvarProp );
			$procs = $store->getPropertyValues( $subject, $procProp );
			$contribs = $store->getPropertyValues( $subject, $contProp );
			$timages = $store->getPropertyValues( $subject, $timageProp );
			$tfiles = $store->getPropertyValues( $subject, $tfileProp );
			$crins = $store->getPropertyValues( $subject, $crinProp );

			$tmpurl = preg_replace('/page\//', '', $url);
			$urlDV = SMWDataValueFactory::newTypeIdValue ('_uri', $tmpurl);
			try {
				$execs = $store->getPropertySubjects( $wtempProp, $urlDV->getDataItem() );
			}
			catch (Exception $e) { $execs = array(); }

			$details["url"] = $url;
			foreach($dvars as $dvar) 
				$details['datavariables'][] = $this->getUri($dvar);
			foreach($pvars as $pvar) 
				$details['paramvariables'][] = $this->getUri($pvar);
			foreach($procs as $proc) 
				$details['processes'][] = $this->getUri($proc);
			foreach($contribs as $cont) 
				$details['contributor'] = $this->getUri($cont);
			foreach($timages as $timage) 
				$details['templateimage'] = $this->getUri($timage);
			foreach($tfiles as $tfile) 
				$details['wingstemplate'] = $this->getUri($tfile);
			foreach($crins as $crin) 
				$details['createdin'] = $this->getUri($crin);
			foreach($execs as $exec) 
				$details['executions'][] = $exec->getDbKey();
			return $details;
		}
		return null;
	}
}

