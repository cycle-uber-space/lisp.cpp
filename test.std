
;;(def std (make-env *env*))
;;
;;(with-env std
;;  (load-file "std.lisp"))
;;
;;(println 'pi: (with-env std pi))

(load-file "std.lisp")

(defun test-report (exp val res)
  (if (equal val res)
      (println 'PASS exp '=> res)
      (println 'FAIL exp '=> val '!= res)))

(defmacro test (exp res)
  `(test-report ',exp ,exp ',res))

(test nil nil)
(test t t)

(test 'foo foo)

(test (if nil 'a 'b) a)
(test (if 'eq 'a 'b) a)

(test (equal '(a . b) '(a . b)) t)

(test (cons 'a 'b) (a . b))

(test (car (cons 'a 'b)) a)
(test (cdr (cons 'a 'b)) b)
