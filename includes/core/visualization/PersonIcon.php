<?php
require_once 'UserIcon.php';

class PersonIcon extends UserIcon{
	
   public function __construct() {
   		parent::__construct(true);
   		$this->size = 25;
   }
   
   protected function isMultiple(){
   		return false;
   }
  
}