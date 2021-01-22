(import 'sdl.tl)

(define cell-px 2)
(define cell-units .01)
(define cells 200)
(define draw-treshold (* cell-units 1.415))

(define (abs x) (if (< x 0) (- x) x))
(define (abs- . l) (abs (apply - l)))

(define (cell-to-unit x) (* cell-units (- x (/ cells 2))))

(define (fill-cell x y)
  (fill-rect (* cell-px x) (* cell-px y) cell-px cell-px 255 255 255))

(define (calc-lhs-rhs-diff lhs rhs x y)
  (abs- (eval lhs) (eval rhs)))

(define (plot-cell lhs rhs x y)
  (if (< (calc-lhs-rhs-diff rhs lhs (cell-to-unit x) (- (cell-to-unit y)))
          draw-treshold)
      (fill-cell x y)
      ()))

(define (plot lhs rhs)
  (open-window 'Plot (* cell-px cells) (* cell-px cells))
  (define (iter-x x)
    (define (iter-y y)
      (if (= y cells) 
          () 
          {(plot-cell lhs rhs x y) (iter-y (+ 1 y))}))
    (if (= x cells)
        ()
        {(iter-y 0) (iter-x (+ 1 x))}))
  (iter-x 0))

(define (plot-bool-cell f x y)
  (if (f (cell-to-unit x) (- (cell-to-unit y))) (fill-cell x y) ()))
