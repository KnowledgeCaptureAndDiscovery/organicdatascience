<?php
require_once 'MetaIcon.php';

class ExpertiseIcon extends MetaIcon{

   	public function __construct($fade) {
       	parent::__construct($fade, false);
   	}
	
	protected function renderSvg(){
		$rx = $this->innerSize()/2;
		$ry = $rx*.3;
		$cx = $this->cx();
		$cy = $this->cy();
		$svg = '<ellipse cx="'.$cx.'" cy="'.$cy.'" rx="'.$rx.'" ry="'.$ry.'" fill="none" stroke-width="2" stroke="'.$this->lightColor().'" />';
		$svg .= '<ellipse cx="'.$cx.'" cy="'.$cy.'" rx="'.$rx.'" ry="'.$ry.'" fill="none" stroke-width="2" stroke="'.$this->lightColor().'" transform="rotate(120 '.$cx.','.$cy.')"/>';
		$svg .= '<ellipse cx="'.$cx.'" cy="'.$cy.'" rx="'.$rx.'" ry="'.$ry.'" fill="none" stroke-width="2" stroke="'.$this->lightColor().'" transform="rotate(240 '.$cx.','.$cy.')"/>';
		return $svg;
	}

}