<?php
require_once 'UserIcon.php';

class ParticipantIcon extends UserIcon{

	public function __construct($fade) {
		parent::__construct($fade);
	}

	protected function isMultiple(){
		return true;
	}


}