<?php
require_once 'Icon.php';

class DelIcon extends Icon{
	
	public function __construct() {
		$this->setIconHeight(11);	
		$this->setIconWidth(11);	
	}
		
	protected function renderSvg(){
		$size = 6;
		$border = 2;			
		$svg = '<path d="M'.$border.' '.$border.' l'.$size.' '.$size.'" stroke="red" stroke-width="3" stroke-linecap="round"/>';
		$svg .= '<path d="M'.($size+$border).' '.$border.' l'.-$size.' '.$size.'" stroke="red" stroke-width="3" stroke-linecap="round"/>';
		return $svg;
	}	
	
}