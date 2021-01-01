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

(define (list . l) l)

(define (first l) (car l))
(define (prog1 . l) (first l))

(define (last l) (if (cdr l)
                     (last (cdr l))
                     (car l)))

(defmacro (@ fun list)
  `(eval (cons ,fun ,list)))

(define (+ . l)
  (progn
    (define (iter result left)
      (if left
          (iter (plus result (car left)) (cdr left))
          result))
    (iter 0 l)))

(define (- . l)
  (if l
      (if (cdr l)
          (minus (car l) (@ + (cdr l)))
          (minus 0 (car l)))
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

(define (fib n)
  (progn
    (define (iter i a b) 
      (progn (print a)
        (if (= i n) 
            a 
            (iter (+ 1 i) b (+ a b)))))
    (iter 0 0 1)))

(define (yes sym)
  (progn
    (print sym)
    (yes sym)))

(define (print-yes)
  (progn
    (define yes 'yes)
    (print yes)))
