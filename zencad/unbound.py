#!/usr/bin/env python3

import multiprocessing
import os
import sys
import time
from signal import SIGTERM

from zencad.application import MainWindow
from zencad.viewadaptor import GeometryWidget
import zencad.opengl
import zencad.rpc

import zencad.spawn

from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QThread

def application_starter(pid, r_id, w_sync, tpl):
	from zencad.application import MainWindow

	#init noqt context
	cr1, cw1 = os.pipe()
	cr2, cw2 = os.pipe()
	zencad.spawn.make_clean_spawner(cr1, cw2)
	clean_spawner_ctransler = zencad.rpc.NoQtTransler(cr2, cw1)

	#get winid from creator thread 
	winid = int(os.read(r_id, 512).decode("utf-8"))
	os.close(r_id)

	app = QApplication([])
	ctransler = zencad.rpc.ApplicationNode(*tpl)
	print("create mainwindow")
	mw = MainWindow()
	mw.clean_spawner_ctransler = clean_spawner_ctransler 
	
	def stopworld():
		mw.broadcast_send("stopworld")
		ctransler.stop()
		app.quit()

	mw.add_view_by_id(winid, ctransler, pid)
	mw.show()
	os.write(w_sync, "sync".encode("utf-8"))
	os.close(w_sync)

	print("app.exec()")
	app.lastWindowClosed.connect(stopworld)
	app.exec()


class update_loop(QThread):
	def __init__(self, parent, updater_function, wdg, pause_time=0.01):
		QThread.__init__(self, parent)
		self.updater_function = updater_function 
		self.wdg = wdg
		self.pause_time = pause_time

	def run(self):
		while 1:
			ensave = zencad.lazy.encache 
			desave = zencad.lazy.decache
			onplace = zencad.lazy.onplace
			diag = zencad.lazy.diag
			if self.wdg.inited:
				zencad.lazy.encache = False
				zencad.lazy.decache = False
				zencad.lazy.onplace = True
				zencad.lazy.diag = False
				self.updater_function(self.wdg)
				zencad.lazy.onplace = onplace
				zencad.lazy.encache = ensave
				zencad.lazy.decache = desave
				zencad.lazy.diag = diag
				time.sleep(self.pause_time)

def animate_stub(wdg):
	wdg.view.redraw()

def start_unbound(scn, animate=None):
	pid = os.getpid()
	r_id, w_id = os.pipe()
	r_sync, w_sync = os.pipe()

	cr1, cw1 = os.pipe()
	cr2, cw2 = os.pipe()

	ctransler = zencad.rpc.EvaluatorNode(cr1, cw2)

	appproc = multiprocessing.Process(target = application_starter, args=(pid, r_id, w_sync, (cr2, cw1)))
	appproc.start()


	app = QApplication([])
	zencad.opengl.init_opengl()
	disp = GeometryWidget(scn)
	os.write(w_id, str(int(disp.winId())).encode("utf-8"))
	os.close(w_id)
	os.read(r_sync, 512)
	os.close(r_sync)

	def stopworld():
		ctransler.stop()
		app.quit()

	ctransler.stopWorldSignal.connect(stopworld)
	ctransler.actionCenteringSignal.connect(disp.action_centering)
	ctransler.actionAutoscaleSignal.connect(disp.action_autoscale)
	ctransler.actionResetSignal.connect(disp.action_reset)
	
	if animate != None:
		thr = update_loop(disp, animate, disp)
		thr.start()
	else:
		#thr = update_loop(disp, animate_stub, disp)
		#thr.start()
		pass

	disp.show()
	app.exec()


def start_self(scn):
	app = QApplication([])
	#zencad.opengl.init_opengl()
	disp = GeometryWidget(scn)
	disp.show()
	app.exec()


def start_self(scn):
	app = QApplication([])
	zencad.opengl.init_opengl()
	disp = GeometryWidget(scn)
	disp.show()
	app.exec()