<?php
require_once 'UserIcon.php';

class PersonOwnerIcon extends UserIcon{
	
   public function __construct($isOwner) {
   		parent::__construct(true);
   		$this->size = 13;
   		$this->isOwner = $isOwner;
   }
   
   protected function isMultiple(){
   		return !$this->isOwner;
   }
  
}