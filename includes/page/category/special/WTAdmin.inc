<?php

class WTAdmin extends WTBase {
	
	function __construct($title) {
		parent::__construct($title);
	}

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();		

		# Admin Div
		$this->addAdminDiv( $out );
		
		# -- Add Existing Markup Here
		$out->addHTML( $old );

		# -- Sidebar Div
		$this->addSidebarDiv( $out );
				
		$this->addYandexMetrica( $out );
	}


}

