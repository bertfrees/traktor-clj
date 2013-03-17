(ns traktor.test
  (:use [clojure.test]
        [traktor.midi]))

(defn- approx [x y & more]
  (apply = (map #(* 0.01 (Math/round (* 100 (float %)))) (apply vector x y more))))

(deftest test-master-tempo
  (testing "Master tempo is 128 BPM"
    (is (approx 128 (master-tempo)))))