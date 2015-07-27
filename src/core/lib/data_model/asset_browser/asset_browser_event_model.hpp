//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_event_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ASSET_BROWSER_EVENT_MODEL_H_
#define ASSET_BROWSER_EVENT_MODEL_H_

#pragma once

#include "i_asset_browser_event_model.hpp"

#include <vector>

// Bare bones signal implementation
// Future improvements could include:
//	Thread safety
//	disconnect
//	auto-disconnect/scoped disconnect
template<class TFunc>
class Signal
{
public:
	void operator()()
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot();
		}
	}

	template<class TArg1>
	void operator()(TArg1 arg1)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1);
		}
	}

	template<class TArg1, class TArg2>
	void operator()(TArg1 arg1, TArg2 arg2)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1, arg2);
		}
	}

	template<class TArg1, class TArg2, class TArg3>
	void operator()(TArg1 arg1, TArg2 arg2, TArg3 arg3)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1, arg2, arg3);
		}
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4>
	void operator()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1, arg2, arg3, arg4);
		}
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
	void operator()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg5 arg5)
	{
		for ( auto slot : m_slots )
		{
			if ( slot )
				slot(arg1, arg2, arg3, arg4, arg5);
		}
	}

	void connect(std::function<TFunc> slot)
	{
		m_slots.emplace_back(std::move(slot));
	}

	Signal& operator+=( std::function<TFunc> slot )
	{
		m_slots.emplace_back(std::move(slot));
		return *this;
	}

private:
	std::vector<std::function<TFunc>> m_slots;
};

typedef Signal<void(const Variant&)> SignalVariant;
typedef Signal<void(bool)> SignalBool;
typedef Signal<void(void)> SignalVoid;

class AssetBrowserEventModel : public IAssetBrowserEventModel
{
public:
	virtual void assetSelectionChanged(const Variant& selection) override;
	virtual void breadcrumbSelected(const Variant& breadcrumb) override;
	virtual void contextMenu(const Variant& menu) override;
	virtual void filterChanged(const Variant& filter) override;
	virtual void folderSelectionChanged(const Variant& folderSelection) override;
	virtual void navigateHistoryForward(const bool& val) override;
	virtual void navigateHistoryBackward(const bool& val) override;
	virtual void useSelectedAsset(const Variant& asset) override;
	
	virtual void connectAssetSelectionChanged(VariantCallback) override;
	virtual void connectBreadcrumbSelected(VariantCallback) override;
	virtual void connectContextMenu(VariantCallback) override;
	virtual void connectFilterChanged(VariantCallback) override;
	virtual void connectFolderSelectionChanged(VariantCallback) override;
	virtual void connectNavigateHistoryForward(VoidCallback) override;
	virtual void connectNavigateHistoryBackward(BoolCallback) override;
	virtual void connectUseSelectedAsset(VariantCallback) override;

private:
	SignalVariant onAssetSelectionChanged;
	SignalVariant onBreadcrumbSelected;
	SignalVariant onContextMenu;
	SignalVariant onFilterChanged;
	SignalVariant onFolderSelectionChanged;
	SignalVoid onNavigateHistoryForward;
	SignalBool onNavigateHistoryBackward;
	SignalVariant onUseSelectedAsset;
};
#endif // ASSET_BROWSER_EVENT_MODEL_H_