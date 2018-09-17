<?php

class WTMainPage extends WTBase {
	var $limit = 10;

	function __construct($title) {
		parent::__construct($title);
	}

//	function setJavascriptVariables($out) {
//	}
//
//	function includeJSHeaders($out) {
//	}

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();


		global $wgUseSimpleTasks;
		if($wgUseSimpleTasks) {
			# Recent Changes
			$this->addRecentChangedPages( $out, $this->limit );
			# Most Recent Contributors
			$this->addRecentContributors( $out, $this->limit/2 );
		}

		# Highest Contributors
		$this->addHighestContributors( $out, $this->limit/2 );

		# -- Add Existing Markup Here
		$out->addHTML( $old );

		# -- Sidebar Div
		$this->addSidebarDiv( $out );

		$this->addYandexMetrica( $out );

		# -- CategoryChooser Div
		if(!$wgUseSimpleTasks) {
			$this->addCategoryChooserDiv( $out );
		}
	}


	function addRecentChangedPages($out, $limit) {
		$out->addHTML("<div class='wt-sidebox props-table'>");
		$out->addHTML("<div class='heading'><b>Recently Changed Pages</b></div>");
		$out->addHTML("<div style='padding:5px'>");
		$out->addHTML($this->getRecentChangesHTML($limit));
		$out->addHTML("</div>");
		$out->addHTML("</div>");
	}

	function addRecentContributors($out, $limit) {
		$out->addHTML("<div class='wt-sidebox props-table'>");
		$out->addHTML("<div class='heading'><b>Recent Contributors</b></div>");
		$out->addHTML("<div style='padding:5px'>");
		$out->addHTML($this->getRecentContributorsHTML($limit));
		$out->addHTML("</div>");
		$out->addHTML("</div>");
	}

	function addHighestContributors($out, $limit) {
		$out->addHTML("<div class='wt-sidebox props-table'>");
		$out->addHTML("<div class='heading'><b>Highest Contributors</b></div>");
		$out->addHTML("<div style='padding:5px'>");
		$out->addHTML($this->getHighestContributorsHTML($limit));
		$out->addHTML("</div>");
		$out->addHTML("</div>");
	}

	function modifySidebar(&$sidebar) {
		$limit = $this->limit;
		//$sidebar["Recently Changed Pages"] = $this->getRecentChangesHTML($limit);
		$sidebar["Highest Contributors"] = $this->getHighestContributorsHTML($limit);
		$sidebar["Recent Contributors"] = $this->getRecentContributorsHTML($limit);
	}

	function getRecentContributorsHTML($limit) {
		$users = $this->getRecentContributors($limit);
		$html = "<ul>";
		foreach($users as $user) {
			$user_text = str_replace("_", " ", $user);
			$html .= "<li><a href='./User:$user'>$user_text</a></li>";
		}
		$html .= "</ul>";
		return $html;
	}

	function getHighestContributorsHTML($limit) {
		$users = $this->getHighestContributors($limit);
		$html = "<ul>";
		foreach($users as $user) {
			$user_text = str_replace("_", " ", $user[0]);
			$html .= "<li><a href='./User:".$user[0]."'>$user_text</a> (".$user[1]." Edits)</li>";
		}
		$html .= "</ul>";
		return $html;
	}

	function getRecentChangesHTML($limit) {
		$pages = $this->getRecentlyChangedPages($limit);
		$html = "<ul>";
		foreach($pages as $page) {
			$page_text = str_replace("_", " ", $page);
			$html .= "<li><a href='$page'>$page_text</a></li>";
		}
		$html .= "</ul>";
		return $html;
	}


	function getRecentlyChangedPages($limit) {
		$dbr = wfGetDB( DB_SLAVE );
		$res = $dbr->select(
			'page',
			array( 'page_title'),
			array( 'page_namespace = 0' ),
			__METHOD__,
			array(
				'ORDER BY' => "page_touched DESC",
				'LIMIT' => "$limit"
			)
		);
		$changedPages = array();
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while($row = $dbr->fetchObject( $res ))
				$changedPages[] = $row->page_title;
		}
		return $changedPages;
	}

	function getRecentContributors($limit) {
		$dbr = wfGetDB( DB_SLAVE );
		$res = $dbr->select(
			'revision',
			array( 'distinct rev_user_text'),
			array( 'rev_user > 0'),
			__METHOD__,
			array(
				'ORDER BY' => "rev_timestamp DESC",
				'LIMIT' => "$limit"
			)
		);
		$recentContribs = array();
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while($row = $dbr->fetchObject( $res ))
				$recentContribs[] = $row->rev_user_text;
		}
		return $recentContribs;
	}

	function getHighestContributors($limit) {
		$dbr = wfGetDB( DB_SLAVE );
		$res = $dbr->select(
			'revision',
			array( 'rev_user_text', 'COUNT(*) total'),
			array( 'rev_user > 0'),
			__METHOD__,
			array(
				'GROUP BY' => "rev_user",
				'ORDER BY' => "total DESC",
				'LIMIT' => "$limit"
			)
		);
		$highestContribs = array();
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while($row = $dbr->fetchObject( $res ))
				$highestContribs[] = array($row->rev_user_text, $row->total);
		}
		return $highestContribs;
	}
}
