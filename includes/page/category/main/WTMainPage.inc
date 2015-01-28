<?php

class WTMainPage extends WTBase {
	var $limit = 10;

	function __construct($title) {
		parent::__construct($title);
	}

//	function setJavascriptVariables($out) {
//	}
//
//	function includeJSHeaders($out, $wgDir) {
//	}

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();

		global $wgUseSimpleTasks;
		if($wgUseSimpleTasks) {
			# Recent Changes
			$this->addRecentChangedPages( $out, $this->limit );
			# Most Recent Contributors
			$this->addRecentContributors( $out, $this->limit );
		}

		# Highest Contributors
		$this->addHighestContributors( $out, $this->limit );

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
		$changedPages = $this->getRecentlyChangedPages($limit);
		$out->addHTML("<div class='wt-sidebox props-table'>");
		$out->addHTML("<div class='heading'><b>Recently Changed Pages</b></div>");
		$out->addHTML("<div style='padding:5px'>");
		$out->addHTML("<ul>");
		foreach($changedPages as $page) {
			$page_text = str_replace("_", " ", $page);
			$out->addHTML("<li><a href='$page'>$page_text</a></li>");
		}
		$out->addHTML("</ul>");
		$out->addHTML("</div>");
		$out->addHTML("</div>");
	}

	function addRecentContributors($out, $limit) {
		$recentContribs = $this->getRecentContributors($limit);
		$out->addHTML("<div class='wt-sidebox props-table'>");
		$out->addHTML("<div class='heading'><b>Recent Contributors</b></div>");
		$out->addHTML("<div style='padding:5px'>");
		$out->addHTML("<ul>");
		foreach($recentContribs as $user) {
			$user_text = str_replace("_", " ", $user);
			$out->addHTML("<li><a href='./User:$user'>$user_text</a></li>");
		}
		$out->addHTML("</ul>");
		$out->addHTML("</div>");
		$out->addHTML("</div>");
	}

	function addHighestContributors($out, $limit) {
		$highestContribs = $this->getHighestContributors($limit);
		$out->addHTML("<div class='wt-sidebox props-table'>");
		$out->addHTML("<div class='heading'><b>Highest Contributors</b></div>");
		$out->addHTML("<div style='padding:5px'>");
		$out->addHTML("<ul>");
		foreach($highestContribs as $user) {
			$user_text = str_replace("_", " ", $user[0]);
			$out->addHTML("<li><a href='./User:".$user[0]."'>$user_text</a> (".$user[1]." Edits)</li>");
		}
		$out->addHTML("</ul>");
		$out->addHTML("</div>");
		$out->addHTML("</div>");
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
		$recentContribs = array();
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while($row = $dbr->fetchObject( $res ))
				$recentContribs[] = array($row->rev_user_text, $row->total);
		}
		return $recentContribs;
	}
}


