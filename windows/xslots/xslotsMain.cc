

#include "xslots.h"
#include "srvlog.h"

#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )

xslots_param_t xp = { 0 };


int main(int argc, char* argv[])
{
	res_file_t sok = NULL;
	xhand_t bio = NULL;
	xhand_t pipe = NULL;
	dword_t dw;

	int i, len;

	tchar_t sz_cert[RES_LEN] = { 0 };

	tchar_t errcode[NUM_LEN + 1] = { 0 };
	tchar_t errtext[ERR_LEN + 1] = { 0 };

	xdl_process_init(XDL_APARTMENT_PROCESS);

	TRY_CATCH;

	get_runpath(NULL, xp.sz_root, PATH_LEN);

	for (i = 1; i < argc; i++)
	{
		len = xslen(xp.sz_param);
#ifdef _UNICODE
		mbs_to_ucs((const schar_t*)argv[i], -1, xp.sz_param + len, PATH_LEN - len);
#else
		a_xsncpy(xp.sz_param + len, argv[i], PATH_LEN - len);
#endif
		xsncat(xp.sz_param, _T(" "), 1);
	}

	get_param_item(xp.sz_param, _T("CERT"), sz_cert, RES_LEN);

	if (compare_text(sz_cert, -1, _T("SSL"), -1, 1) == 0)
		xp.n_secu = _SECU_SSL;
	else if (compare_text(sz_cert, -1, _T("XSL"), -1, 1) == 0)
		xp.n_secu = _SECU_XSL;
	else
		xp.n_secu = _SECU_NONE;

	pipe = xpipe_srv(NULL, 0);

	if (!pipe)
	{
		raise_user_error(_T("-1"), _T("child process create std pipe failed"));
	}

	dw = 0;
	sok = xsocket_dupli(xpipe_handle(pipe), FILE_OPEN_OVERLAP, NULL, &dw);

	if (sok == INVALID_FILE)
	{
		raise_user_error(_T("-1"), _T("child process create socket server failed"));
	}

	xpipe_free(pipe);
	pipe = NULL;

	if (xp.n_secu == _SECU_SSL)
		bio = xssl_srv(sok);
	else if (xp.n_secu == _SECU_XSL)
		bio = xxsl_srv(sok);
	else
		bio = xtcp_srv(sok);

	if (!bio)
	{
		raise_user_error(_T("-1"), _T("child process create tcp server failed"));
	}

	_xslots_dispatch(bio, &xp);

	if (xp.n_secu == _SECU_SSL)
		xssl_close(bio);
	else if (xp.n_secu == _SECU_XSL)
		xxsl_close(bio);
	else
		xtcp_close(bio);

	bio = NULL;

	xsocket_close(sok);
	sok = INVALID_FILE;

	END_CATCH;

	xdl_process_uninit();

	return 0;

ONERROR:

	get_last_error(errcode, errtext, ERR_LEN);

	if (sok != INVALID_FILE)
		xsocket_close(sok);

	if (pipe)
		xpipe_free(pipe);

	if (bio)
	{
		if (xp.n_secu == _SECU_SSL)
			xssl_close(bio);
		else if (xp.n_secu == _SECU_XSL)
			xxsl_close(bio);
		else
			xtcp_close(bio);
	}

	xdl_process_uninit();

	return -1;
}

