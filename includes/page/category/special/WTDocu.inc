<?php

class WTDocu extends WTBase {
	
	function __construct($title) {
		parent::__construct($title);
	}

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();		

		# -- Add Existing Markup Here
		$out->addHTML( $old );

		# -- Sidebar Div
		$this->addSidebarDiv( $out );
				
		$this->addYandexMetrica( $out );
	}


}

