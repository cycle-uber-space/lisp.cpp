
;;(def pi 3.141592654)

(def defun (syntax (name args . body)
             `(def ,name (lambda ,args ,@body))))

(def defmacro (syntax (name args . body)
                `(def ,name (syntax ,args ,@body))))

(defun caar (exp)
  (car (car exp)))

(defun cadr (exp)
  (car (cdr exp)))

(defun cdar (exp)
  (cdr (car exp)))

(defun cddr (exp)
  (cdr (cdr exp)))

(defun caddr(exp)
  (car (cdr (cdr exp))))

(defun cadddr (exp)
  (car (cdr (cdr (cdr exp)))))

(defun not (arg)
  (if arg
      nil
      t))

(defun append (a b)
  (if a
      (cons (car a)
            (append (cdr a) b))
      b))

(defun list args
  (append args nil))

(defun map (fun seq)
  (if seq
      (cons (fun (car seq))
            (map fun (cdr seq)))))

(defmacro let (decls . body)
  `((lambda ,(map car decls) ,@body) ,@(map cadr decls)))

(defmacro let* (decls . body)
  (defun expand (decls)
    (if decls
        (list 'let (list (car decls))
              (expand (cdr decls)))
        (cons 'progn body)))
  (expand decls))

(defmacro cond clauses
  (defun expand (clauses)
    (if clauses
        (let* ((clause (car clauses))
               (others (cdr clauses))
               (test (car clause))
               (body (cdr clause))
               (then (cons 'progn body)))
          (if others
              (list 'if test then (expand others))
              (list 'if test then)))))
  (expand clauses))

(defmacro progn body
  `(let () ,@body))

(defmacro when (test . body)
  `(if ,test (progn ,@body)))

(defmacro unless (test . body)
  `(when (not ,test) ,@body))

(defun + args
  (apply number-+ args))

(defun - args
  (apply number-- args))

(defun * args
  (apply number-* args))

(defun / args
  (apply number-/ args))
