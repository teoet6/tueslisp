(defsym defmacro (macro (sig body)
  `(defsym ,(car sig) (macro ,(cdr sig) ,body))))

(defmacro (defun name params body)
  `(defsym ,name (lambda ,params ,body)))

(defmacro (define sig body)
  (if (pair? sig)
      `(defun ,(car sig) ,(cdr sig) ,body)
      `(defsym ,sig ,body)))

(define t 't)
(define nil ())
(define nl '\n)

(define (list . l) l)

(define (first l) (car l))
(define (prog1 . l) (first l))

(define (last l) (if (cdr l)
                     (last (cdr l))
                     (car l)))

(defmacro (apply fun list)
  `(eval (cons ,fun ,list)))

(define (- . l)
  (if l
      (if (cdr l)
          (minus (car l) (apply + (cdr l)))
          (minus 0 (car l)))
      0))          

(define (* . l)
  (progn
    (define (iter result left)
      (if left
          (iter (multiply result (car left)) (cdr left))
          result))
    (iter 1 l)))

(define (/ . l)
  (if l
      (if (cdr l)
          (divide (car l) (apply * (cdr l)))
          (divide 0 (car l)))
      1))

(define (% . l)
  (if l
      (- (car l)
         (* (whole-part (apply / l))
            (apply * (cdr l))))
      0))

(define (= . l)
  (progn
    (define (iter x l)
      (if l
          (if (equal x (car l))
              (iter x (cdr l))
              ())
          t))
    (iter (car l) (cdr l))))

(defmacro (cond . cases)
  (if cases
      `(if ,(car (car cases))
           ,(car (cdr (car cases)))
           (apply cond ',(cdr cases)))
      ()))

(defmacro (import filename) `(eval-file (open-file ',(cat filename '.tl))))

(define (yes sym)
  (progn
    (print sym)
    (yes sym)))

(define (fib n)
  (progn
    (define (iter i a b) 
      (progn (print a)
        (if (= i n) 
                    a 
            (iter (+ 1 i) b (+ a b)))))
    (iter 0 0 1)))

(define (! n) (if (= n 0) 1 (* n (! (- n 1)))))

(define (loop start end) (if (= start end) end (loop (+ 1 start) end)))