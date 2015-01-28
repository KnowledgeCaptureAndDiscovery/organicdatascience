<?php
require_once 'MetaIcon.php';

class ProgressIcon extends MetaIcon{
	
   	public function __construct($fade) {
       	parent::__construct($fade, false);
   	}
		
	protected function renderSvg(){
		$this->init();		
		$x = $this->border;
		$w = $this->innerSize()*.66;
		$this->barElement($x, $w, $this->darkColor());
		$this->barElement($x+$w, $this->innerSize()-$w, $this->lightColor());					
		return $this->svg;
	}
	
	private function init(){
		$this->height = $this->innerSize()/2;
		$this->y = $this->size/2-$this->height/2;
		$this->svg = '';
	}	

	private function barElement($x, $w, $c){	 
		$this->svg .= '<rect x="'.$x.'" y="'.$this->y.'" width="'.$w.'" height="'.$this->height.'" fill="'.$c.'" />';	
	}
}