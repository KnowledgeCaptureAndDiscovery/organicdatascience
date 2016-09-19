<?php

class WTNewPerson extends WTBase {
	
	function __construct($title) {
		parent::__construct($title);
	}

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();
		
		# WTNewPerson Div
		$this->addPersonDiv( $out );

		$out->addHTML($old);

		$this->addYandexMetrica( $out );
	}

	function setJavascriptVariables($out) {
		parent::setJavascriptVariables($out);
	}

	function getPageDetails() {
		return $this->getWTNewPerson();
	}

	function getWTNewPerson() {
		$userid = $this->title->getText();
		$details = array(
			"wikiuser" => $this->getUserWikiDetails( $userid ),
			"persons" => $this->findAllPersons()
		);
		return $details;
	}

	function getUserWikiDetails( $user ) {
		$dbr = wfGetDB( DB_SLAVE );
		$uf = ucfirst($user);
		$res = $dbr->select(
			array( 'user', 'user_groups' ),
			array( 'user_real_name', 'user_email', 'user_registration', 'group_concat(ug_group) as groups' ),
			array( "user_name = '$uf'"),
			__METHOD__,
			array( ),
			array( 'user_groups' => array( 'INNER JOIN', array('user_id=ug_user') ) )
		);
		$details = array();
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while($row = $dbr->fetchObject( $res ))
				$details = array(
					"id"=>$uf,
					"name"=>$row->user_real_name,
					"email"=>$row->user_email,
					"since"=>$row->user_registration,
					"groups"=>$row->groups);
		}
		return $details;
	}

	public static function findAllPersons(){	
		global $wgCore;
		$persons = array();
		$expertise = array();
		
		$query = "[[Category:Person_$wgCore]]";
		$rawparams = array($query, "?Has_User_ID", "limit=9999");
	
		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );		

		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);	
		$results = smwfGetStore()->getQueryResult( $queryobj );
				
		$res = $results->getNext();
		while($res != null) {
			if(sizeof($res) == 2) 
				$person_id = null;
				$person_name = null;
				while($exp= $res[0]->getNextObject()) 					
					$person_name=$exp->getText();
				while($exp= $res[1]->getNextObject()) 					
					$person_id=$exp->getShortWikiText();
				$persons[$person_name] = $person_id;
			$res = $results->getNext();
		}
		return $persons;
	}

	public static function getPersonNameFromUserId($userid) {
		$title = Title::newFromText("User:".$userid);
		$store = &smwfGetStore();
		$userid = $title->getDbKey();
		$useridProp = new SMWDIProperty('Has_User_ID');
		$uidDV = SMWDataValueFactory::newTypeIdValue('_txt', $userid);
		try {
			$vals = $store->getPropertySubjects( $useridProp, $uidDV->getDataItem() );
			foreach($vals as $val) {
				return $val->getDbKey();
			}
		}
		catch (Exception $e) { $execs = array(); }
	}
}

