#!/home/teoet6/docs/tues/9/prog/tueslisp/tueslisp
(import fizzbuzz)

# Opredeleno trqbva da ima sym2ascii i ascii2sym funkcii
# Tova e bavnichko
(define (sym-to-digit sym)
  (progn
    (define carsym (car sym))
    (cond ((= carsym '\0) 0)
          ((= carsym '\1) 1)
          ((= carsym '\2) 2)
          ((= carsym '\3) 3)
          ((= carsym '\4) 4)
          ((= carsym '\5) 5)
          ((= carsym '\6) 6)
          ((= carsym '\7) 7)
          ((= carsym '\8) 8)
          ((= carsym '\9) 9))))

(define (sym-to-num sym)
  (progn
    (define (iter sym val)
      (if sym
          (iter (cdr sym) (+ (* 10 val)
                             (sym-to-digit sym)))
          val))
    (iter sym 0)))
          
(define n (sym-to-num (car (cdr command-line-args))))

(fizzbuzz n)
