<?php
require_once 'MetaIcon.php';

class LockIcon extends MetaIcon{

	public function __construct() {
		parent::__construct(true, false);
	}

	protected function renderSvg(){	
		$width = $this->innerSize()*.6;
		$x = $this->cx() - $width/2;
		$y = $this->border + $this->innerSize()*.33;
		$r = $width/2;
		$svg  = '<circle cx="'.$this->cx().'" cy="'.$y.'" r="'.$r.'" fill="'.$this->darkColor().'" />';
		$svg .= '<circle cx="'.$this->cx().'" cy="'.$y.'" r="'.($r*.6).'" fill="white" />';
		$svg .= '<rect x="'.$x.'" y="'.$y.'" width="'.$width.'" height="'.$width.'" fill="'.$this->darkColor().'" />';			
		return $svg;
	}


}