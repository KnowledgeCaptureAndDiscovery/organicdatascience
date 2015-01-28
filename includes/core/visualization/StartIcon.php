<?php
require_once 'TimeIcon.php';

class StartIcon extends TimeIcon{
	
   	public function __construct($fade, $warning) {
       	parent::__construct($fade, $warning);
   	}
	
	protected function isLeft(){
		return true;
	}

}