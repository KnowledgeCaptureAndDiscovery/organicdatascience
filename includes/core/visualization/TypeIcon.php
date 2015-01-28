<?php
require_once 'MetaIcon.php';

class TypeIcon extends MetaIcon{
	
   	public function __construct($fade, $warning) {
       	parent::__construct($fade, $warning);
   	}
		
	protected function renderSvg(){
		$cx = $this->cx(); /** Mitte oben */ 
		$cy = $this->border;		
		$l1x = -$this->innerSize()/2;
		$l1y = $this->innerSize();
		$l2x = $l1y;
		$l2y = 0;
		$d="M $cx $cy l $l1x $l1y l $l2x $l2y Z";
		$svg = '<path fill="'.$this->lightColor().'" d="'.$d.'" />'; 
		return $svg;
	}
}