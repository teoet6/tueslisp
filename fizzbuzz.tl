(define (fizzbuzz n)
  (progn
    (define (fizzbuzz-q x)
      (cond ((= 0 (% x 15)) 'fizzbuzz)
            ((= 0 (% x 3)) 'fizz)
            ((= 0 (% x 5)) 'buzz)
            (t x)))
    (define (iter i)
      (if (= i (+ 1 n))
          ()
          (progn
            (print (fizzbuzz-q i) nl)
            (iter (+ 1 i)))))
    (iter 1)))
