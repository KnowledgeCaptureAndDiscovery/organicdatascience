<?php

class WTWorkingGroup extends WTBase {
	static $topWg = "Working_Group";

	function __construct($title) {
		parent::__construct($title);
	}

	function modifyWikiPage($out) {
		global $wgScriptPath;

		# -- Add Information Box if no type defined
		$old = $out->getHTML();
		$parts = preg_split('/__MEMBERS__/', $old, 2);
		$part1 = sizeof($parts) > 1 ? $parts[0] : "";
		$part2 = sizeof($parts) > 1 ? $parts[1] : $parts[0];

		$out->clearHTML();
		$out->addHTML($part1);
		$wgname = $this->title->getText();
		$users = WTWorkingGroup::getMembersOfWorkingGroup($wgname);
		sort($users);

		$out->addHTML("<div lang='en-GB' dir='ltr'><div id='mw-pages'>");
		$out->addHTML("<h2>Members of '$wgname'</h2><ul>");
		if(sizeof($users) == 0)
			$out->addHTML('<p>This working group has no members.</p>');
		else if(sizeof($users) == 1)
			$out->addHTML('<p>This working group contains only the following member.</p>');
		else 
			$out->addHTML('<p>This working group has '.sizeof($users).' members.</p>');

		$ausers = array();
		foreach($users as $user) { 
			$letter = $user[0];
			if(array_key_exists($letter, $ausers))
				$ausers[$letter][] = $user;
			else
				$ausers[$letter] = [$user];
		}
		$out->addHTML('<div lang="en-GB" dir="ltr" class="mw-content-ltr">');
		foreach($ausers as $letter=>$lusers) {
			$out->addHTML("<h3>$letter</h3>");
			$out->addHTML("<ul>");
			foreach($lusers as $user)
				$out->addHTML("<li><a href='$wgScriptPath/index.php/$user' title='$user'>$user</a></li>");
			$out->addHTML("</ul>");
		}
		$out->addHTML("</div>");
		$out->addHTML("</div></div>");

		$old = preg_replace("/Pages in category/", "Pages in", $old);
		$old = preg_replace("/category/", "working group", $old);
		$out->addHTML($part2);
	}

	public static function getAllWorkingGroups() {
		$dbr = wfGetDB( DB_SLAVE );
		$res = $dbr->select(
			array( 'page', 'categorylinks' ),
			array( 'page_title', 'page_namespace' ),
			array( "cl_to='".WTWorkingGroup::$topWg."'" , "page_namespace=".NS_CATEGORY ),
			__METHOD__,
			array(),
			array(
				'categorylinks' => array( 'LEFT JOIN', array('page_id=cl_from') )
			)
		);  
		$groups = array(); 
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while($row = $dbr->fetchObject( $res ))
				$groups[] = Title::makeName($row->page_namespace, $row->page_title);
		}           
		return $groups;
    } 

	public static function addWorkingGroupToWatchlist($person, $propid, $group) {
		$ptitle = Title::newFromText($person);
		$gtitle = Title::newFromText($group);
		$userid = WTPerson::getUserId($ptitle);
		$user = User::newFromName($userid);
		if(!$user)
			return false;

		// Add the workgroup itself to the watchlist
		if(!$user->isWatched($gtitle))
			$user->addWatch($gtitle);

		// Add all pages in the workgroup to the watchlist
		$cat = Category::newFromTitle($gtitle);
		$members = $cat->getMembers();
		while( $members->valid() ) {
			if(!$user->isWatched($members->current))
				$user->addWatch($members->current);
			$members->next();
		}
	}

	public static function removeWorkingGroupFromWatchlist($person, $propid, $group) {
		$ptitle = Title::newFromText($person);
		$gtitle = Title::newFromText($group);
		$userid = WTPerson::getUserId($ptitle);
		$user = User::newFromName($userid);
		if(!$user)
			return false;

		// Remove the workgroup itself from the watchlist
		if($user->isWatched($gtitle))
			$user->removeWatch($gtitle);

		// Remove all pages in the workgroup from the watchlist
		$cat = Category::newFromTitle($gtitle);
		$members = $cat->getMembers();
		while( $members->valid() ) {
			if($user->isWatched($members->current))
				$user->removeWatch($members->current);
			$members->next();
		}
	}

	public static function getMembersOfWorkingGroup($group) {
		$users = array();
		$store = &smwfGetStore();
		$gpage = SMWDIWikiPage::newFromText($group, NS_CATEGORY);
		$propDI = SMWDIProperty::newFromUserLabel("Subscribes To");
		$userpages = $store->getPropertySubjects( $propDI, $gpage );
		foreach ( $userpages as $userpage )
			$users[] = $userpage->getDbKey();
		return $users;
	}

	public static function isWorkingGroup($group) {
		$groupfull = Title::makeName(NS_CATEGORY, $group);
		$cats = WTFactsAPI::getPageCategories($groupfull);
		if(in_array(WTWorkingGroup::$topWg, $cats))
			return true;
		return false;
	}
	
}
?>
