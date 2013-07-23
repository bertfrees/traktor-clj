(ns traktor.midi.native
  (:use [clj-native.direct :only [defclib loadlib]])
  (:require [clj-native.callbacks])
  (:refer-clojure :exclude (load)))

(defclib traktor-midi
  (:libname "traktor_midi")
  (:callbacks
    (void-function [] void)
    (double-function [double] void))
  (:functions
    (master-tempo master_tempo [double-function] void-function)))

(def loaded (atom false))

(defn load []
  (when (not @loaded)
    (reset! loaded true)
    (loadlib traktor-midi)
    (Thread/sleep 500)))

;; To make sure callbacks are not garbage collected!
(def active-callbacks (atom ()))

(defn- create-callback [type callback]
  (let [native-callback (clj-native.callbacks/callback type callback)]
    (swap! active-callbacks conj native-callback)
    native-callback))

(defn double-callback [callback]
  (create-callback double-function callback))

(defn discard-callback [callback]
  (swap! active-callbacks #(remove #{callback} %)))

(defn invoke-callback [callback]
  (.invoke callback))
