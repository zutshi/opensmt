(set-logic QF_LRA)
(declare-fun v () Real)
(assert (or (= 0.0 (* (/ 1 120030) v)) (= 0.0 v)))
(check-sat)
(exit)
