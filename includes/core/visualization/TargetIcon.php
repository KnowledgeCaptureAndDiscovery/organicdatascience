<?php
require_once 'TimeIcon.php';

class TargetIcon extends TimeIcon{
	
   	public function __construct($fade, $warning) {
       	parent::__construct($fade, $warning);
   	}
	
	protected function isLeft(){
		return false;
	}

}