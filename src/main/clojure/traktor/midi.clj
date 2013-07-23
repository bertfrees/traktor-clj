(ns traktor.midi
  (:use [traktor.collection])
  (:require [traktor.midi.native :as native]))

;; ------------ ;;
;; MASTER TEMPO ;;
;; ------------ ;;

(defn master-tempo-watch
  ([callback]
     (native/load)
     (let [native-callback (native/double-callback callback)
           native-cancel (native/master-tempo native-callback)]
       (fn [] (native/invoke-callback native-cancel)
         (native/discard-callback native-callback))))
  ([callback sensitivity]
     (if (> sensitivity 0)
       (let [tempo (atom 0)]
         (master-tempo-watch
          #(when (< sensitivity (Math/abs (- @tempo %)))
             (reset! tempo %)
             (callback %))))
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

;; ----- ;;
;; DECKS ;;
;; ----- ;;

(defn track-title-stream [deck callback]
  (native/load)
  (let [native-callback (native/char-callback #(callback (char %)))
        native-cancel (native/track-title ({:a 0 :b 1 :c 2 :d 3} deck) native-callback)]
      (fn [] (native/invoke-callback native-cancel)
             (native/discard-callback native-callback))))

(defn track-title-watch [deck callback]
  (let [buffer (StringBuffer.)
        title (atom nil)
        new-title (atom nil)
        successive-spaces (atom 0)]
    (track-title-stream deck
      (fn [character]
        (if (= character \space)
          (swap! successive-spaces inc)
          (do (when (> @successive-spaces 2)
                (let [s (.substring buffer 0 (- (.length buffer) @successive-spaces))]
                  (when (and (= s @new-title) (not (= s @title)))
                    (reset! title s)
                    (callback s))
                  (reset! new-title s))
                (.delete buffer 0 (.length buffer)))
              (reset! successive-spaces 0)))
        (.append buffer character)))))

(defn track-artist-stream [deck callback]
  (native/load)
  (let [native-callback (native/char-callback #(callback (char %)))
        native-cancel (native/track-artist ({:a 0 :b 1 :c 2 :d 3} deck) native-callback)]
      (fn [] (native/invoke-callback native-cancel)
             (native/discard-callback native-callback))))

(defn track-artist-watch [deck callback]
  (let [buffer (StringBuffer.)
        artist (atom nil)
        new-artist (atom nil)
        successive-spaces (atom 0)]
    (track-artist-stream deck
      (fn [character]
        (if (= character \space)
          (swap! successive-spaces inc)
          (do (when (> @successive-spaces 2)
                (let [s (.substring buffer 0 (- (.length buffer) @successive-spaces))]
                  (when (and (= s @new-artist) (not (= s @artist)))
                    (reset! artist s)
                    (callback s))
                  (reset! new-artist s))
                (.delete buffer 0 (.length buffer)))
              (reset! successive-spaces 0)))
        (.append buffer character)))))

(defn track-watch [deck callback]
  (let [title (atom nil)
        artist (atom nil)
        track (atom nil)
        refresh #(let [new-track (if (not-any? nil? [@title @artist])
                                   (first (find-tracks {:title @title :artist @artist})))]
                   (when (not (= new-track @track))
                     (reset! track new-track)
                     (if @track (callback @track))))
        cancel-title-watch (track-title-watch deck #(do (reset! title %) (refresh)))
        cancel-artist-watch (track-artist-watch deck #(do (reset! artist %) (refresh)))]
    #(do (cancel-title-watch)
         (cancel-artist-watch))))

(defn track-agent [deck]
  (let [track (agent nil)]
    (track-watch deck #(send track (constantly %)))
    track))

(defn track [deck]
  (let [track (promise)
        cancel (track-watch deck #(deliver track %))
        track (deref track 25000 nil)]
    (cancel)
    (or track (throw (RuntimeException. "timeout")))))
