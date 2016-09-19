<?php

class WTPerson extends WTBase {
	
	function __construct($title) {
		parent::__construct($title);
	}

	/*function setJavascriptVariables($out) {
		parent::setJavascriptVariables($out);
		$out->addHTML("<script>var wtpagetype = 'WTPerson';</script>\n");
	}*/

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();
		
		# WTPersonExpertise Div
		$this->addPersonExpertiseDiv( $out );
		
		# WTPersonTaks Div
		$this->addPersonTasksDiv( $out );
		
		# WTPerson Div
		$this->addPersonDiv( $out, true );

		# -- Add Existing Markup Here
		$out->addHTML( $old );

		# -- Add the Facts Table Here
		//$this->addMainFactsDiv( $out );
		$this->addInPropsDiv($out);

		# -- Credits Div
		$this->addCreditsDiv( $out );

		# -- Sidebar Div
		$this->addSidebarDiv( $out );
		
		# -- CategoryChooser Div 
		$this->addCategoryChooserDiv( $out ); 
				
		$this->addYandexMetrica( $out );
	}

	function getPageDetails() {
		$details = parent::getPageDetails();
		$details["WTPerson"] = $this->getWTPerson();
		return $details;
	}

	public static function getUserId($title) {
		$userid = "";
		$store = &smwfGetStore();
		$subject = SMWDIWikiPage::newFromTitle( $title );
		$semdata = $store->getSemanticData( $subject );
		$useridProp = new SMWDIProperty('Has_User_ID');
		$userids = $semdata->getPropertyValues($useridProp);
		foreach($userids as $uidobj) {
			$userid = $uidobj->getString();
			break;
		}
		return $userid;
	}

	function getWTPerson() {
		$details = array(
			'contributions'=>array(),
			'allgroups'=>array()
			//'expertise'=>$this->getExpertise()
		);
		$userid = "";
		if($this->title->getNamespace() == NS_USER) {
			// If this is a User:[userid] type of page
			$userid = $this->title->getText();
		}
		else {
			// If this is a normal page with Category Person
			$userid = WTPerson::getUserId($this->title);
		}

		//$details["contributions"] = $this->getRecentContributions( $userid, 1000 );
		$details["wikiuser"] = $this->getUserWikiDetails( $userid );
		if($details["wikiuser"]["id"])
			$details["contributions"] = $this->getUserContributions( $details["wikiuser"]["id"] );
		else
			$details["contributions"] = array();
		$details["allgroups"] = WTWorkingGroup::getAllWorkingGroups();
		return $details;
	}

	function getUserWikiDetails( $user ) {
		$dbr = wfGetDB( DB_SLAVE );
		$uf = ucfirst($user);
		$res = $dbr->select(
			array( 'user', 'user_groups' ),
			array( 'user_id', 'user_real_name', 'user_email', 'user_registration', 'group_concat(ug_group) as groups' ),
			array( "user_name = '$uf'"),
			__METHOD__,
			array( ),
			array( 'user_groups' => array( 'INNER JOIN', array('user_id=ug_user') ) )
		);
		$details = array();
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while($row = $dbr->fetchObject( $res ))
				$details = array(
					"id"=>$row->user_id,
					"name"=>$row->user_real_name,
					"email"=>$row->user_email,
					"since"=>$row->user_registration,
					"groups"=>preg_split("/,/", $row->groups));
		}
		return $details;
	}

	function getRecentContributions($user, $limit) {
		$dbr = wfGetDB( DB_SLAVE );
		$uf = ucfirst($user);
		$res = $dbr->select(
			array( 'revision', 'page' ),
			array( 'page_title', 'page_namespace', 'rev_comment', 'rev_timestamp' ),
			array( "rev_user_text = '$uf'"),
			__METHOD__,
			array( 
				'ORDER BY' => "rev_timestamp DESC",
				'LIMIT' => "$limit"
			),
			array( 'page' => array( 'INNER JOIN', array('rev_page=page_id') ) )
		);
		$recentContribs = array();
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while($row = $dbr->fetchObject( $res ))
				$recentContribs[] = array(
					Title::makeName($row->page_namespace, $row->page_title),
					$row->rev_comment, 
					$row->rev_timestamp);
		}
		return $recentContribs;
	}

	function getUserContributions( $userid ) {
		$dbr = wfGetDB( DB_SLAVE );
		$res = $dbr->select(
			array( 'revision', 'page', 'categorylinks' ),
			array( 'page_title', 'page_namespace', "cl_to 'page_category'",
				"CASE WHEN rev_parent_id = 0 THEN 'Create' ELSE 'Edit' END 'operation'", 
				"CASE WHEN rev_comment LIKE 'WTBootstrapLE%' THEN 'Bootstrap' WHEN rev_comment LIKE 'WTLiPD%' THEN 'LiPD' ELSE 'User' END 'agent'",
				"count(*) count" ),
			array( "rev_user=$userid"),
			__METHOD__,
			array( 
				'GROUP BY' => "page_namespace, page_title, operation"
			),
			array( 
				'page' => array( 'LEFT JOIN', array('rev_page=page_id') ),
				'categorylinks' => array( 'LEFT JOIN', array('rev_page=cl_from') ) 
			)
		);
		$userContribs = array();
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while($row = $dbr->fetchObject( $res ))
				$userContribs[] = array(
					Title::makeName($row->page_namespace, $row->page_title),
					$row->page_category, 
					$row->operation, 
					$row->count, 
					$row->agent);
		}
		return $userContribs;
	}
	
	function getExpertise() {
		$query = "[[Category:Person]] [[".$this->title."]]";
		$rawparams = array($query, "?Expertise");
	
		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );		

		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);	
		$results = smwfGetStore()->getQueryResult( $queryobj );
				
		$res = $results->getNext();
		while($res != null) {
			if(sizeof($res) == 2) {
				$expertise = array();
				while($person = $res[0]->getNextObject()) {					
					if($person->getText() == $this->title){
						while($exp = $res[1]->getNextObject())
							$expertise[] = $exp->getText();
						return $expertise;
					}
				}
			}			
			$res = $results->getNext();
		}
		return null;		
	}
	
	public static function findAllExpertise(){	
		$persons = array();
		$expertise = array();
		
		$query = "[[Category:Person]]";
		$rawparams = array($query, "?Expertise", "limit=9999");
	
		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );		

		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);	
		$results = smwfGetStore()->getQueryResult( $queryobj );
				
		$res = $results->getNext();
		while($res != null) {
			if(sizeof($res) == 2) 
				while($exp= $res[0]->getNextObject()) 					
					$persons[$exp->getText()] = true;		
				while($exp= $res[1]->getNextObject()) 					
					$expertise[strtolower($exp->getText())] = true;		
			$res = $results->getNext();
		}
		return array(array_keys($persons), array_keys($expertise));		
	}

	public static function getPersonNameFromUserId($userid) {
		$title = Title::newFromText($userid, NS_USER);
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

	public static function watchPage($user, $pagetitle) {
		$ptitle = Title::newFromText($user);
		$userid = WTPerson::getUserId($ptitle);
        $user = User::newFromName($userid);
        if(!$user)
            return false;
        if(!$user->isWatched($pagetitle))
            $user->addWatch($pagetitle);
		return true;
	}

	public static function unwatchPage($user, $pagetitle) {
		$ptitle = Title::newFromText($user);
		$userid = WTPerson::getUserId($ptitle);
        $user = User::newFromName($userid);
        if(!$user)
            return false;
        if($user->isWatched($pagetitle))
            $user->removeWatch($pagetitle);
		return true;
	}
}

