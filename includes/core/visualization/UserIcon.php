<?php
require_once 'MetaIcon.php';

abstract class UserIcon extends MetaIcon{
	
   public function __construct($fade) {
   		parent::__construct($fade, false);
   }
   
   protected abstract function isMultiple();
   
	protected function renderSvg(){
		$r1 = (3/14)*$this->innerSize();
		$r2 = (5/14)*$this->innerSize();
		$cx = $this->cx();
		$cy = $this->border+$r1;
		$svg = '';
		if($this->isMultiple()){
			$svg .= $this->user($cx-3, $cy, $r1, $r2, $this->darkColor());
			$svg .= $this->user($cx+3, $cy+2, $r1, $r2, $this->mediumColor()); 
		}
		$svg .= $this->user($cx, $cy+3, $r1, $r2, $this->lightColor()); 
		return $svg;
	}
	
	private function user($cx, $cy, $r1, $r2, $c){
		$cy-=3;
		$svg = '<circle cx="'.$cx.'" cy="'.$cy.'" r="'.$r1.'" fill="'.$c.'" />';
		$cy += $r2+(2/14)*$this->innerSize();
		$svg .= '<circle cx="'.$cx.'" cy="'.$cy.'" r="'.$r2.'" fill="'.$c.'" />';
		$w = 2*$r2;
		$x = $cx-$r2;
		$h = max($r2,5);
		$svg .= '<rect x="'.$x.'" y="'.$cy.'" width="'.$w.'" height="'.$h.'" fill="'.$c.'" />';
		return $svg;
	}
}