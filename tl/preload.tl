(defsym defmacro (macro (sig . body)
  `(defsym ,(car sig) (macro ,(cdr sig) . ,body))))

(defmacro (defun name params . body)
  `(defsym ,name (lambda ,params . ,body)))

(defmacro (define sig . body)
  (if (pair? sig)
      `(defun ,(car sig) ,(cdr sig) . ,body)
     `(defsym ,sig . ,body)))

(define t 't)
(define nil ())
(define nl '\n)
(define space '\ )

(define (list . l) l)

(define (first l) (car l))
(define (prog1 . l) (first l))

(define (last l) (if (cdr l)
                     (last (cdr l))
                     (car l)))

(defmacro (apply fun list)
  `(eval (cons ,fun ,list)))

(define (% . l)
  (if l
      (- (car l)
         (* (whole-part (apply / l))
            (apply * (cdr l))))
      0))

(define (= . l)
  (define (iter x l)
    (if l
        (if (equal x (car l))
            (iter x (cdr l))
            ())
        t))
  (iter (car l) (cdr l)))

(defmacro (and . l)
  (if l
      `(if ,(car l)
           (apply and ',(cdr l))
           ())
      t))

(define (<= x y) (or (< x y) (= x y)))
(define (>= x y) (or (> x y) (= x y)))

(defmacro (or . l)
  (if l
      `(if ,(car l)
           t
           (apply or ',(cdr l)))
      ()))

(defmacro (cond . cases)
  (if cases
      `(if ,(car (car cases))
           ,(car (cdr (car cases)))
           (apply cond ',(cdr cases)))
      ()))

(define (yes sym)
  (print sym)
  (yes sym))

(define (fib n)
  (define (iter i a b) 
    (print a nl)
      (if (= i n) 
          a 
          (iter (+ 1 i) b (+ a b))))
  (iter 0 0 1))

(define (! n) (if (= n 0) 1 (* n (! (- n 1)))))

(define (loop start end) (if (= start end) end (loop (+ 1 start) end)))
