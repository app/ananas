/****************************************************************************
** Thin compatibility wrapper around QProcess, replacing the Qt3 Q3Process
** API used by the Ananas container/report code.
**********************************************************************/

#ifndef APROCESS_H
#define APROCESS_H

#include <QProcess>
#include <QString>
#include <QStringList>

class aProcess
{
public:
	explicit aProcess( const QString &program = QString() )
		: m_program( program ) {}

	void setWorkingDirectory( const QString &dir ) { m_dir = dir; }
	void addArgument( const QString &arg ) { m_args << arg; }

	bool start()
	{
		if ( !m_dir.isEmpty() )
			m_process.setWorkingDirectory( m_dir );
		m_process.start( m_program, m_args );
		return m_process.waitForStarted();
	}

	void waitForFinished() { m_process.waitForFinished( -1 ); }

	bool isRunning() const { return m_process.state() != QProcess::NotRunning; }
	bool normalExit() const { return m_process.exitStatus() == QProcess::NormalExit; }
	int exitStatus() const { return m_process.exitCode(); }

private:
	QString    m_program;
	QStringList m_args;
	QString    m_dir;
	QProcess   m_process;
};

#endif // APROCESS_H
