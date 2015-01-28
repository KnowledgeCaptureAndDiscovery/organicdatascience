<?php
require_once 'UserIcon.php';

class OwnerIcon extends UserIcon{

	public function __construct($fade) {
		parent::__construct($fade);
	}

	protected function isMultiple(){
		return false;
	}


}