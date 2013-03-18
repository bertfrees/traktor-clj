(ns traktor.midi
  (:use [overtone.midi]))

(defn- traktor-virtual-output [] (midi-in (midi-find-device (midi-sources) "Traktor Virtual Output")))

(defn master-tempo-watch
  ([callback]
    (let [clock (traktor-virtual-output)
          cancel #(let [transmitter (:transmitter clock)]
                    (.close (.getReceiver transmitter))
                    (.setReceiver transmitter nil))
          timestamps (atom ())]
      (midi-handle-events clock
        (fn [msg]
          (if (= (:status msg) :timing-clock)
            (if (> (count @timestamps) 50)
              (let [tempo (->> (- (:timestamp msg) (apply min @timestamps))
                               (/ (count @timestamps))
                               (* (/ 60000000 24))
                               float)]
                (callback tempo)
                (swap! timestamps empty))
              (swap! timestamps conj (:timestamp msg))))))
      cancel))
  ([callback sensitivity]
    (if (> sensitivity 0)
      (let [tempo (atom 0)]
        (master-tempo-watch
          #(when (< sensitivity (Math/abs (- @tempo %)))
             (reset! tempo %)
             (callback @tempo))))
      (master-tempo-watch callback))))

(defn master-tempo-agent []
  (let [tempo (agent nil)]
    (master-tempo-watch #(send tempo (constantly %)))
    tempo))

(defn master-tempo []
  (let [tempo (promise)
        cancel (master-tempo-watch #(deliver tempo %))
        tempo (deref tempo 10000 nil)]
    (cancel)
    (or tempo (throw (RuntimeException. "timeout")))))
