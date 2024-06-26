#include "APIRequest.h"
#include "client/Client.h"

namespace http
{
	APIRequest::APIRequest(ByteString url, AuthMode authMode, bool newCheckStatus) : Request(url), checkStatus(newCheckStatus)
	{
		auto user = Client::Ref().GetAuthUser();
		if (authMode == authRequire && !user.UserID)
		{
			FailEarly("Not authenticated");
			return;
		}
		if (authMode != authOmit && user.UserID)
		{
			AuthHeaders(ByteString::Build(user.UserID), user.SessionID);
		}
	}

	Json::Value APIRequest::Finish()
	{
		auto [ status, data ] = Request::Finish();
		ParseResponse(data, status, checkStatus ? responseJson : responseData);
		Json::Value document;
		try
		{
			std::istringstream ss(data);
			ss >> document;
		}
		catch (const std::exception &ex)
		{
			throw RequestError("\u65e0\u6cd5\u8bfb\u53d6\u54cd\u5e94\u003a\u0020" + ByteString(ex.what()));
		}
		return document;
	}
}
